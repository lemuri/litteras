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

#include "EwsFolderModel.h"
#include "EwsEngine.h"

#include <EwsQt5/syncfolderhierarchyreply.h>
#include <EwsQt5/connection.h>
#include <EwsQt5/reply.h>

#include <QStringBuilder>
#include <QStandardPaths>

#include <QDebug>

using namespace Ews;

EwsFolderModel::EwsFolderModel(const QString &location, EwsEngine *parent) :
    QStandardItemModel(parent),
    m_parent(parent),
    m_updateTimer(new QTimer(this))
{
    m_uuid = parent->uuid();

    m_configName = location % QLatin1String("/") % m_uuid;
    m_settings = new QSettings(m_configName, QSettings::IniFormat);

    m_roleNames[RoleFolderId]       = "roleFolderId";
    m_roleNames[RoleFolderParentId] = "roleFolderParentId";
    m_roleNames[RoleChangeKey]      = "roleChangeKey";
    m_roleNames[RoleDisplayName]    = "roleDisplayName";
    m_roleNames[RoleHasChildren]    = "roleHasChildren";
    m_roleNames[RoleExpanded]       = "roleExpanded";
    m_roleNames[RoleDisplayName]    = "roleDisplayName";

    // Update the list in one minute
    connect(m_updateTimer, SIGNAL(timeout()), SLOT(sync()));
    m_updateTimer->setInterval(10000);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->start();

    init();
}

bool EwsFolderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    qDebug() << Q_FUNC_INFO << index << value << role;
    if (role == Qt::EditRole) {
//        Ews::Folder folder(m_parent->connection(),
//                         index.data(RoleFolderId).toString(),
//                         index.data(RoleChangeKey).toString());
//        folder.setDisplayName(value.toString());
//        Ews::Reply *reply = folder.update();
//        connect(reply, SIGNAL(finished()), SLOT(updateFolderFinished()));
    } else if (role == RoleExpanded) {
        int depth = index.data(RoleDepth).toInt();
        bool expanded = value.toBool();
        for (int i = index.row() + 1; i < rowCount(); ++i) {

            QStandardItem *stdItem = item(i);
            qDebug() << Q_FUNC_INFO << i << expanded << stdItem->data(RoleDepth).toInt() << depth;
            if (stdItem->data(RoleDepth).toInt() <= depth) {
                break;
            }
            qDebug() << Q_FUNC_INFO << stdItem->data(RoleDisplayName).toString() << !expanded;
            stdItem->setData(!expanded, RoleExpanded);
        }
    }

    return false;
}

QStringList EwsFolderModel::folderIds(const QModelIndex &parent) const
{
    QStringList ret;
    for (int i = 0; i < rowCount(parent); ++i) {
        QModelIndex folderIndex = index(i, 0, parent);
        if (!folderIndex.isValid()) {
            continue;
        }

        ret << folderIndex.data(RoleFolderId).toString();

        ret << folderIds(folderIndex);
    }

    return ret;
}

void EwsFolderModel::init()
{
    int size = m_settings->beginReadArray("Folders");
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);

        addFolderItem(m_settings->value("FolderId").toString(),
                      m_settings->value("ParentId").toString(),
                      m_settings->value("ChangeKey").toString(),
                      m_settings->value("DisplayName").toString());
    }
    m_settings->endArray();

    sync();
}

void EwsFolderModel::sync()
{
    qDebug() << Q_FUNC_INFO << m_configName;
    qDebug() << Q_FUNC_INFO << m_parent;
    qDebug() << Q_FUNC_INFO << m_parent->connection();
    qDebug() << Q_FUNC_INFO << m_parent->connection()->serverVersion();

    m_settings->beginGroup("SyncState");
    QString lastSyncState = m_settings->value("SyncState").toString();
    m_settings->endGroup();

    Ews::SyncFolderHierarchyReply *reply = m_parent->connection()->syncFolderHierarch(Ews::Folder::AllProperties,
                                                                                    QString(),
                                                                                    lastSyncState);
    qDebug() << Q_FUNC_INFO << reply;
    connect(reply, SIGNAL(finished()), SLOT(syncFolderHierarchyFinished()));
}

void EwsFolderModel::syncFolderHierarchyFinished()
{
    beginResetModel();


    Ews::SyncFolderHierarchyReply *response = qobject_cast<Ews::SyncFolderHierarchyReply*>(sender());
//    qDebug() << Q_FUNC_INFO << response->messageText() << response->errorMessage();
    if (response->error()) {
        qDebug() << Q_FUNC_INFO << "SyncFolderHierarchyReply failed" << response->errorMessage();
        return;
    }

    foreach (const Folder &folder, response->createFolders()) {
        addFolder(folder);
        emit syncItems(folder.id());
    }

    foreach (const Folder &folder, response->updateFolders()) {
        addFolder(folder);
    }

    foreach (const QString &folderId, response->deleteFolders()) {
        deleteFolder(folderId);
    }

    m_settings->beginGroup("SyncState");
    QString lastSyncState = m_settings->value("SyncState").toString();
    if (lastSyncState != response->syncState()) {
        m_settings->setValue("SyncState", response->syncState());
    }
    m_settings->endGroup();

    endResetModel();
}

void EwsFolderModel::updateFolderFinished()
{
//    EwsReply *response = qobject_cast<EwsReply*>(sender());
//    if (response->error()) {
//        kDebug() << "updateFolderFinished failed" << response->errorMessage();
//        response->deleteLater();
//    } else {
//        sync();
    //    }
}

void EwsFolderModel::addFolder(const Ews::Folder &folder)
{
    qDebug() << Q_FUNC_INFO << folder.displayName() << folder.id();
    QStandardItem *stdItem = findItem(folder.id());
    if (!stdItem) {
        int size = m_settings->beginReadArray("Folders");
        m_settings->endArray();

        m_settings->beginWriteArray("Folders");
        m_settings->setArrayIndex(size);

        m_settings->setValue("FolderId", folder.id());
        m_settings->setValue("ChangeKey", folder.changeKey());
        m_settings->setValue("ParentId", folder.parentId());
        m_settings->setValue("DisplayName", folder.displayName());

        addFolderItem(folder.id(), folder.parentId(), folder.changeKey(), folder.displayName());

        m_settings->endArray();

        return;
    }

    if (stdItem->data(RoleChangeKey).toString() != folder.changeKey()) {
        // todo update
    }


//    settings.beginWriteArray("Folders");

//    QString storedChangeKey = settings.value("ChangeKey").toString();
//    if (storedChangeKey.isEmpty() || storedChangeKey != folder.changeKey()) {
//        settings.setValue("ChangeKey", folder.changeKey());
//        settings.setValue("ParentId", folder.parentId());
//        settings.setValue("DisplayName", folder.displayName());

//        addFolderItem(folder.id(), folder.parentId(), folder.changeKey(), folder.displayName());
//    }
//    settings.endGroup();// Folder.id

//    settings.endGroup();// Folders
}

void EwsFolderModel::deleteFolder(const QString &folderId)
{
    m_settings->beginGroup("Folders");
    m_settings->remove(folderId);

    QStandardItem *stdItem = findItem(folderId);
    if (stdItem) {
        qDebug() << stdItem->row() << stdItem->parent()->index();
        removeRow(stdItem->row(), stdItem->parent()->index());
    }
}

void EwsFolderModel::addFolderItem(const QString &id, const QString &parentId, const QString &changeKey, const QString &title)
{
    QStandardItem *stdItem = findItem(id);
    if (stdItem) {
        if (stdItem->data(RoleFolderParentId).toString() == parentId) {
            stdItem->setData(title, RoleDisplayName);
            stdItem->setData(changeKey, RoleChangeKey);
            return;
        }

        if (stdItem->parent()) {
            removeRow(stdItem->row(), stdItem->parent()->index());
        } else {
            removeRow(stdItem->row());
        }
    }

    stdItem = new QStandardItem;
    stdItem->setData(title, RoleDisplayName);
    stdItem->setData(id, RoleFolderId);
    stdItem->setData(parentId, RoleFolderParentId);
    stdItem->setData(changeKey, RoleChangeKey);
    stdItem->setData(0, RoleHasChildren);

    QStandardItem *parentItem = findItem(parentId);
    if (parentItem) {
        int children = parentItem->data(RoleHasChildren).toInt();
        parentItem->setData(++children, RoleHasChildren);

        int depth = parentItem->data(RoleDepth).toInt();
        stdItem->setData(++depth, RoleDepth);

        stdItem->setData(false, RoleExpanded);
    } else {
        stdItem->setData(0, RoleDepth);
        stdItem->setData(true, RoleExpanded);
    }
    appendRow(stdItem);
}

QStandardItem *EwsFolderModel::findItem(const QString &id)
{
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *stdItem = item(i, 0);
        if (stdItem->data(RoleFolderId).toString() == id) {
            return stdItem;
        }
    }

    return 0;
}
