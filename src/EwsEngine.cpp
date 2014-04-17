/***************************************************************************
 *   Copyright (C) 2013 Daniel Nicoletti <dantti12@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "EwsEngine.h"

#include <connection.h>
#include <syncfolderhierarchyreply.h>
#include <syncfolderitemsreply.h>

#include "EwsFolderModel.h"

#include <QStandardPaths>
#include <QDnsLookup>
#include <QDir>
#include <QTimer>
#include <QDebug>

using namespace Ews;

EwsEngine::EwsEngine(const QSettings &settings, const QString &uuid, QObject *parent) :
    QObject(parent),
    m_uuid(uuid)
{
    // First thing to do is setup our connection
    m_internalUri = settings.value("internalASUrl").toString();
    m_externalUri = settings.value("externalASUrl").toString();

    QUrl credentials = settings.value("URI").toUrl();
    m_internalUri.setUserInfo(credentials.userInfo());
    m_externalUri.setUserInfo(credentials.userInfo());

    m_connection = new Ews::Connection(this);
    m_connection->setUri(m_internalUri);

    QDir dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (!dataLocation.exists() && !dataLocation.mkpath(dataLocation.absolutePath())) {
        qWarning() << "Failed to create data directory" << dataLocation.absolutePath();
    }

    QString accountName = settings.value("Description").toString();
    if (accountName.isEmpty()) {
        accountName = settings.value("EmailAddress").toString().section(QLatin1Char('@'), 1, 1).section(QLatin1Char('.'), 0, 0);
    }

    m_folderModel = new EwsFolderModel(dataLocation.absolutePath(), this);
    m_folderModel->setProperty("HEADER", accountName.toUpper());
//    connect(m_folderModel, SIGNAL(syncItems(QString)),
//            SLOT(syncItems(QString)));

//    qDebug() << Q_FUNC_INFO << uri.host() << m_uuid;

    checkDNS();

    // Update the list in one minute
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    timer->setInterval(5000);
    timer->start();
}

void EwsEngine::update()
{
    m_folderModel->sync();
//    foreach (const QString &folderId, m_folderModel->folderIds()) {
//        syncItems(folderId);
//    }
}

EwsFolderModel *EwsEngine::folderModel() const
{
    return m_folderModel;
}

Ews::Connection *EwsEngine::connection() const
{
    return m_connection;
}

QString EwsEngine::uuid() const
{
    return m_uuid;
}

void EwsEngine::syncItems(const QString &folderId)
{
    SyncFolderItemsReply *reply = m_connection->syncFolderItems(Folder::AllProperties,
                                                                   folderId,
                                                                   10);
    connect(reply, SIGNAL(finished()), SLOT(syncFolderItemsFinished()));
}

void EwsEngine::syncFolderItemsFinished()
{
    SyncFolderItemsReply *reply = qobject_cast<SyncFolderItemsReply*>(sender());
//    foreach (const Message &message, reply->createMessages()) {
//        emit addMessage(message.id(),
//                        message.parentFolderId(),
//                        message.subject(),
//                        message.from().name(),
//                        message.dateTimeReceived(),
//                        message.isRead());
//        qDebug() << message.subject() << message.dateTimeCreated() << message.dateTimeReceived();
//    }

    reply->deleteLater();
}

void EwsEngine::checkDNS()
{
    qDebug() << Q_FUNC_INFO << m_internalUri.host() << m_externalUri.host();
    QDnsLookup *dns = new QDnsLookup(this);
    connect(dns, &QDnsLookup::finished,
            this, &EwsEngine::checkDNSFinished);

    dns->setProperty("host", m_internalUri.host());
    dns->setType(QDnsLookup::ANY);
    dns->setName(m_internalUri.host());
    dns->lookup();

    dns = new QDnsLookup(this);
    connect(dns, &QDnsLookup::finished,
            this, &EwsEngine::checkDNSFinished);

    dns->setProperty("host", m_externalUri.host());
    dns->setType(QDnsLookup::ANY);
    dns->setName(m_externalUri.host());
    dns->lookup();
}

void EwsEngine::checkDNSFinished()
{
    QDnsLookup *dns = qobject_cast<QDnsLookup*>(sender());
    qDebug() << Q_FUNC_INFO << dns << dns->name() << dns->error() << dns->errorString();
    if (dns->property("host").toString() == m_internalUri.host()) {
        if (dns->error()) {
            m_connection->setUri(m_externalUri);
            update();
        }
    } else {
        if (dns->error()) {
            m_connection->setUri(m_internalUri);
            update();
        }
    }
    dns->deleteLater();
}
