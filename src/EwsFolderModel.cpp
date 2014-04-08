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

//#include <KConfig>
//#include <KConfigGroup>

#include <QDebug>

using namespace Ews;

EwsFolderModel::EwsFolderModel(EwsEngine *parent) :
    QStandardItemModel(parent),
    m_parent(parent),
    m_updateTimer(new QTimer(this))
{
    m_uuid = parent->uuid();
    m_configName = QLatin1String("litteras-") % m_uuid;

    m_roleNames[RoleFolderId]       = "roleFolderId";
    m_roleNames[RoleFolderParentId] = "roleFolderParentId";
    m_roleNames[RoleChangeKey]      = "roleChangeKey";
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
    qDebug() << index << value << role;
    if (role == Qt::EditRole) {
        Ews::Folder folder(m_parent->connection(),
                         index.data(RoleFolderId).toString(),
                         index.data(RoleChangeKey).toString());
        folder.setDisplayName(value.toString());
        Ews::Reply *reply = folder.update();
        connect(reply, SIGNAL(finished()), SLOT(updateFolderFinished()));
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
    QSettings settings(m_configName, QSettings::NativeFormat);

    int size = settings.beginReadArray("Folders");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);

        addFolderItem(settings.value("FolderId").toString(),
                      settings.value("ParentId").toString(),
                      settings.value("ChangeKey").toString(),
                      settings.value("DisplayName").toString());
    }
    settings.endArray();

    sync();
}

void EwsFolderModel::sync()
{
    QSettings settings(m_configName, QSettings::NativeFormat);
    qDebug() << settings.fileName();
    settings.beginGroup("SyncState");
    QString lastSyncState = settings.value("SyncState").toString();
    Ews::SyncFolderHierarchyReply *reply = m_parent->connection()->syncFolderHierarch(Ews::Folder::AllProperties,
                                                                                    QString(),
                                                                                    lastSyncState);
    connect(reply, SIGNAL(finished()), SLOT(syncFolderHierarchyFinished()));
}

void EwsFolderModel::syncFolderHierarchyFinished()
{
    beginResetModel();

    Ews::SyncFolderHierarchyReply *response = qobject_cast<Ews::SyncFolderHierarchyReply*>(sender());
//    if (response->error()) {
//        qDebug() << Q_FUNC_INFO << "SyncFolderHierarchyReply failed" << response->errorMessage();
//        response->deleteLater();
//        return;
//    }

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

    QSettings settings(m_configName, QSettings::NativeFormat);
    settings.beginGroup("SyncState");

    QString lastSyncState = settings.value("SyncState").toString();
    if (lastSyncState != response->syncState()) {
        settings.setValue("SyncState", response->syncState());
    }

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
    QSettings settings(m_configName, QSettings::NativeFormat);

    QStandardItem *stdItem = findItem(folder.id());
    if (!stdItem) {
        int size = settings.beginReadArray("Folders");
        settings.endArray();

        settings.beginWriteArray("Folders");
        settings.setArrayIndex(size);

        settings.setValue("FolderId", folder.id());
        settings.setValue("ChangeKey", folder.changeKey());
        settings.setValue("ParentId", folder.parentId());
        settings.setValue("DisplayName", folder.displayName());

        addFolderItem(folder.id(), folder.parentId(), folder.changeKey(), folder.displayName());

        settings.endArray();

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
    QSettings settings(m_configName, QSettings::NativeFormat);
    settings.beginGroup("Folders");
    settings.remove(folderId);

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

    QStandardItem *parentItem = findItem(parentId);
    if (parentItem) {
        parentItem->appendRow(stdItem);
    } else {
        appendRow(stdItem);
    }
}

QStandardItem *EwsFolderModel::findItem(const QString &id, const QModelIndex &parent)
{
    for (int i = 0; i < rowCount(parent); ++i) {
        QModelIndex item = index(i, 0, parent);
        if (item.data(RoleFolderId).toString() == id) {
            return itemFromIndex(item);
        }

        QStandardItem *stdItem = findItem(id, item);
        if (stdItem) {
            return stdItem;
        }
    }

    return 0;
}
