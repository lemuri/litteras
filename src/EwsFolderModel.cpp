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
#include "foldermodel.h"

#include <EwsQt5/syncfolderhierarchyreply.h>
#include <EwsQt5/getfolderreply.h>
#include <EwsQt5/connection.h>
#include <EwsQt5/reply.h>

#include <QStringBuilder>
#include <QStandardPaths>
#include <QRegularExpression>

#include <QDebug>

using namespace Ews;

EwsFolderModel::EwsFolderModel(const QString &location, EwsEngine *parent) :
    QStandardItemModel(parent),
    m_parent(parent)
{
    m_uuid = parent->uuid();

    m_configName = location % QLatin1String("/") % m_uuid;
    m_settings = new QSettings(m_configName, QSettings::IniFormat);

    init();
}

bool EwsFolderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    qDebug() << Q_FUNC_INFO << index << value << role;
    if (role == Qt::EditRole) {
//        Ews::Folder folder(m_parent->connection(),
//                         index.data(FolderModel::RoleFolderId).toString(),
//                         index.data(FolderModel::RoleChangeKey).toString());
//        folder.setDisplayName(value.toString());
//        Ews::Reply *reply = folder.update();
//        connect(reply, SIGNAL(finished()), SLOT(updateFolderFinished()));
    } else if (role == FolderModel::RoleExpanded) {
        // The parent item is now expanded
        QStandardItem *parentItem = itemFromIndex(index);
        parentItem->setData(value, FolderModel::RoleExpanded);

        // Set the visibility of the child items
        int depth = index.data(FolderModel::RoleDepth).toInt();
        bool parentExpanded = value.toBool();
        for (int i = index.row() + 1; i < rowCount(); ++i) {
            QStandardItem *stdItem = item(i);
            if (stdItem->data(FolderModel::RoleDepth).toInt() <= depth) {
                break;
            }
            stdItem->setData(parentExpanded, FolderModel::RoleIsVisible);
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

        ret << folderIndex.data(FolderModel::RoleFolderId).toString();

        ret << folderIds(folderIndex);
    }

    return ret;
}

void EwsFolderModel::init()
{
    foreach (const QString &group, m_settings->childGroups()) {
        if (!group.startsWith(QLatin1String("folder_"))) {
            continue;
        }

        m_settings->beginGroup(group);
        QString folderId = m_settings->value("FolderId").toString();
        QString parentId = m_settings->value("ParentId").toString();
        QString changeKey = m_settings->value("ChangeKey").toString();
        QString displayName = m_settings->value("DisplayName").toString();
        QString folderClass = m_settings->value("FolderClass").toString();
        m_settings->endGroup();

        if (folderClass == QLatin1String("IPF.Note")) {
            addFolderItem(folderId,
                          parentId,
                          changeKey,
                          displayName);
        }
    }
}

void EwsFolderModel::sync()
{
    qDebug() << Q_FUNC_INFO << m_configName;

    m_settings->beginGroup("SyncState");
    QString lastSyncState = m_settings->value("SyncState").toString();
    m_settings->endGroup();

    SyncFolderHierarchyReply *reply = m_parent->connection()->syncFolderHierarch(Folder::IdOnly,
                                                                                 QString(),
                                                                                 lastSyncState);
    connect(reply, &SyncFolderHierarchyReply::finished,
            this, &EwsFolderModel::syncFolderHierarchyFinished);
}

void EwsFolderModel::syncFolderHierarchyFinished()
{
    beginResetModel();

    SyncFolderHierarchyReply *response = qobject_cast<SyncFolderHierarchyReply*>(sender());
//    qDebug() << Q_FUNC_INFO << response->messageText() << response->errorMessage();
    if (response->error()) {
        qDebug() << Q_FUNC_INFO << "SyncFolderHierarchyReply failed" << response->errorMessage();
        return;
    }

    qDebug() << Q_FUNC_INFO << response->createFolders().size() << response->updateFolders().size() << response->deleteFolders().size();
    QList<Folder> folders;
    foreach (const Folder &folder, response->createFolders()) {
        addFolder(folder);
        folders << folder;
        emit syncItems(folder.id());
    }

    foreach (const Folder &folder, response->updateFolders()) {
        folders << folder;
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

    if (!folders.isEmpty()) {
        // TODO add wellknownfolders
        GetFolderReply *reply = m_parent->connection()->getFolders(folders, Ews::Folder::AllProperties);
        connect(reply, &GetFolderReply::finished,
                this, &EwsFolderModel::getFolderFinished);
    }

    endResetModel();
}

void EwsFolderModel::getFolderFinished()
{
    GetFolderReply *response = qobject_cast<GetFolderReply*>(sender());
//    qDebug() << Q_FUNC_INFO << response->messageText() << response->errorMessage();
    if (response->error()) {
        qDebug() << Q_FUNC_INFO << "SyncFolderHierarchyReply failed" << response->errorMessage();
        return;
    }

    foreach (const Folder &folder, response->folders()) {
        if (folder.folderClass() == QLatin1String("IPF.Note")) {
            addFolder(folder);
        }
    }
}

void EwsFolderModel::addFolder(const Ews::Folder &folder)
{
    QStandardItem *stdItem = findItem(folder.id());
    if (stdItem && stdItem->data(FolderModel::RoleFolderParentId).toString() != folder.parentId()) {
        deleteFolder(folder.id());
    }

    QString normalized = folder.id().replace(QRegularExpression("[\\W]"), QLatin1String("_"));

    m_settings->beginGroup(QLatin1String("folder_") % normalized);
    m_settings->setValue("FolderId", folder.id());
    m_settings->setValue("ChangeKey", folder.changeKey());
    m_settings->setValue("ParentId", folder.parentId());
    m_settings->setValue("DisplayName", folder.displayName());
    m_settings->setValue("FolderClass", folder.folderClass());
    m_settings->endGroup();

    if (!folder.displayName().isNull()) {
        addFolderItem(folder.id(), folder.parentId(), folder.changeKey(), folder.displayName());
    }
}

void EwsFolderModel::deleteFolder(const QString &folderId)
{
    QString normalized = folderId;
    normalized.replace(QRegularExpression("[\\W]"), QLatin1String("_"));
    m_settings->beginGroup(QLatin1String("folder_") % normalized);
    m_settings->remove("");
    m_settings->endGroup();

    QStandardItem *stdItem = findItem(folderId);
    if (stdItem) {
        if (stdItem->data(FolderModel::RoleDepth).toInt()) {
            QStandardItem *parentItem = findItem(stdItem->data(FolderModel::RoleFolderParentId).toString());
            if (parentItem) {
                // If this is the last item colapse the parent
                int childrenCount = parentItem->data(FolderModel::RoleChildrenCount).toInt();
                parentItem->setData(--childrenCount, FolderModel::RoleChildrenCount);
                if (childrenCount == 0 && parentItem->data(FolderModel::RoleExpanded).toBool()) {
                    parentItem->setData(false, FolderModel::RoleExpanded);
                }
            }
        }
        removeRow(stdItem->row());
    }
}

QStandardItem *EwsFolderModel::addFolderItem(const QString &id, const QString &parentId, const QString &changeKey, const QString &title)
{
    QStandardItem *stdItem = findItem(id);
    if (stdItem) {
        // Update our item
        stdItem->setData(title, FolderModel::RoleDisplayName);
        stdItem->setData(changeKey, FolderModel::RoleChangeKey);
        return stdItem;
    }

    stdItem = new QStandardItem;
    stdItem->setData(title, FolderModel::RoleDisplayName);
    stdItem->setData(id, FolderModel::RoleFolderId);
    stdItem->setData(parentId, FolderModel::RoleFolderParentId);
    stdItem->setData(changeKey, FolderModel::RoleChangeKey);
    stdItem->setData(0, FolderModel::RoleChildrenCount);
    stdItem->setData(false, FolderModel::RoleExpanded);

    int startRow = 0;
    int depth = 0;
    bool visible = true;
    QStandardItem *parentItem = findItem(parentId, true);
    if (parentItem) {
        visible = parentItem->data(FolderModel::RoleExpanded).toBool();
        depth = parentItem->data(FolderModel::RoleDepth).toInt() + 1;
        int children = parentItem->data(FolderModel::RoleChildrenCount).toInt();
        parentItem->setData(++children, FolderModel::RoleChildrenCount);

        startRow = parentItem->row() + 1;
    }
    stdItem->setData(visible, FolderModel::RoleIsVisible);
    stdItem->setData(depth, FolderModel::RoleDepth);

    for (int i = startRow; i < rowCount(); ++i) {
        QStandardItem *loopItem = item(i, 0);
        int loopItemDepth = loopItem->data(FolderModel::RoleDepth).toInt();
        if (loopItemDepth == depth) {
            QString displayName = loopItem->data(FolderModel::RoleDisplayName).toString();
            if (displayName.localeAwareCompare(title) > 0) {
                qDebug() << Q_FUNC_INFO << title << i;
                insertRow(i, stdItem);
                return stdItem;
            }
        } else if (loopItemDepth < depth){
            // We hit the simbling of our parent
            insertRow(i, stdItem);
            return stdItem;
        }
    }

    // if we can't find the right location just append it
    appendRow(stdItem);

    return stdItem;
}

QStandardItem *EwsFolderModel::findItem(const QString &id, bool init)
{
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *stdItem = item(i, 0);
        if (stdItem->data(FolderModel::RoleFolderId).toString() == id) {
            return stdItem;
        }
    }

    if (!init) {
        return 0;
    }

    // A last resort for when we are in init mode
    foreach (const QString &group, m_settings->childGroups()) {
        if (!group.startsWith(QLatin1String("folder_"))) {
            continue;
        }

        m_settings->beginGroup(group);
        QString folderId = m_settings->value("FolderId").toString();
        QString parentId = m_settings->value("ParentId").toString();
        QString changeKey = m_settings->value("ChangeKey").toString();
        QString displayName = m_settings->value("DisplayName").toString();
        m_settings->endGroup();
        if (folderId == id) {
            return addFolderItem(folderId,
                                 parentId,
                                 changeKey,
                                 displayName);
        }
    }

    return 0;
}
