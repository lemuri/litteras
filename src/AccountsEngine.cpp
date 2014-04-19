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

#include "AccountsEngine.h"

#include "EwsEngine.h"
#include "EwsFolderModel.h"
#include "messagesindex.h"

#include <QThread>
#include <QDebug>

Q_GLOBAL_STATIC(AccountsEngine, accountsEngine)

AccountsEngine *AccountsEngine::instance()
{
    return accountsEngine;
}

AccountsEngine::AccountsEngine(QObject *parent)
    : QObject(parent)
    , m_accountsModel(new QStandardItemModel(this))
    , m_messagesIndex(new MessagesIndex)
    , m_messagesIndexThread(new QThread(this))
{
    connect(m_messagesIndexThread, &QThread::started, m_messagesIndex, &MessagesIndex::init);
    m_messagesIndex->moveToThread(m_messagesIndexThread);
    m_messagesIndexThread->start();

    QHash<int, QByteArray> roleNames;
    roleNames[Qt::UserRole] = "name";
    m_accountsModel->setItemRoleNames(roleNames);

    configFileChanged();
}

AccountsEngine::~AccountsEngine()
{
    if (m_messagesIndexThread) {
        m_messagesIndexThread->quit();
        m_messagesIndexThread->wait();
    }
}

QAbstractItemModel *AccountsEngine::accountsModel() const
{
    return m_accountsModel;
}

QList<QAbstractItemModel *> AccountsEngine::engineFolderModels()
{
    QList<QAbstractItemModel *> ret;
    foreach (EwsEngine *engine, m_accounts.values()) {
        ret << engine->folderModel();
    }
    return ret;
}

void AccountsEngine::configFileChanged()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("Accounts"));
    foreach (const QString &group, settings.childGroups()) {
        if (m_accounts.contains(group)) {
            continue;
        }

        settings.beginGroup(group);

        EwsEngine *engine = new EwsEngine(settings, group, this);
        connect(engine, &EwsEngine::addMessage,
                this, &AccountsEngine::addMessage);
        m_accounts.insert(group, engine);

        QStandardItem *item = new QStandardItem;
        QString email = settings.value("EmailAddress").toString();
        item->setData(email, Qt::UserRole);
        item->setData(settings.value("URI").toString());
        m_accountsModel->appendRow(item);
        ++m_accountsCount;
        emit countChanged();

        if (!m_hadAccounts) {
            m_hadAccounts = true;
            emit hadAccountsChanged();
        }

        settings.endGroup();
    }
}
