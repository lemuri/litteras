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

#include <EwsConnection.h>
#include <EwsSyncFolderHierarchyReply.h>
#include <EwsSyncFolderItemsReply.h>

#include "EwsFolderModel.h"

#include <QDebug>

EwsEngine::EwsEngine(const QSettings &settings, const QString &uuid, QObject *parent) :
    QObject(parent),
    m_uuid(uuid)
{

    // First thing to do is setup our connection
    QUrl uri = settings.value("ASUrl").toString();
    QUrl credentials = settings.value("URI").toUrl();
    uri.setUserInfo(credentials.userInfo());
    m_connection = new EwsConnection(this);
    m_connection->setUri(uri);

    m_folderModel = new EwsFolderModel(this);
    connect(m_folderModel, SIGNAL(syncItems(QString)),
            SLOT(syncItems(QString)));

//    qDebug() << Q_FUNC_INFO << uri.host() << m_uuid;

    update();
}

void EwsEngine::update()
{
    foreach (const QString &folderId, m_folderModel->folderIds()) {
        syncItems(folderId);
    }
}

EwsFolderModel *EwsEngine::folderModel() const
{
    return m_folderModel;
}

EwsConnection *EwsEngine::connection() const
{
    return m_connection;
}

QString EwsEngine::uuid() const
{
    return m_uuid;
}

void EwsEngine::syncItems(const QString &folderId)
{
    EwsSyncFolderItemsReply *reply = m_connection->syncFolderItems(EwsFolder::AllProperties,
                                                                   folderId,
                                                                   10);
    connect(reply, SIGNAL(finished()), SLOT(syncFolderItemsFinished()));
}

void EwsEngine::syncFolderItemsFinished()
{
    EwsSyncFolderItemsReply *reply = qobject_cast<EwsSyncFolderItemsReply*>(sender());
    foreach (const EwsMessage &message, reply->createMessages()) {
        emit addMessage(message.id(),
                        message.parentFolderId(),
                        message.subject(),
                        message.from().name(),
                        message.dateTimeReceived(),
                        message.isRead());
        qDebug() << message.subject() << message.dateTimeCreated() << message.dateTimeReceived();
    }

    reply->deleteLater();
    qDebug() << reply->document().toString(2);
}
