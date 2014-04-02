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

#include <EwsQt5/EwsSyncFolderHierarchyReply.h>
#include <EwsQt5/EwsConnection.h>
#include <EwsQt5/EwsReply.h>

#include <QStringBuilder>

//#include <KConfig>
//#include <KConfigGroup>

#include <QDebug>

EwsFolderModel::EwsFolderModel(EwsEngine *parent) :
    QStandardItemModel(parent),
    m_parent(parent),
    m_updateTimer(new QTimer(this))
{
    m_uuid = parent->uuid();
    m_configName = QLatin1String("litteras-") % m_uuid;

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
        EwsFolder folder(m_parent->connection(),
                         index.data(RoleFolderId).toString(),
                         index.data(RoleChangeKey).toString());
        folder.setDisplayName(value.toString());
        EwsReply *reply = folder.update();
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
    settings.beginGroup(QLatin1String("Folders"));
//    KConfig config(m_configName, KConfig::SimpleConfig);
//    KConfigGroup foldersGroup(&config, QLatin1String("Folders"));

    initFolders(settings.allKeys());

    sync();
}

void EwsFolderModel::initFolders(const QStringList &folderIds)
{
    if (folderIds.isEmpty()) {
        return;
    }

//    KConfig config(m_configName, KConfig::SimpleConfig);
//    KConfigGroup foldersGroup(&config, QLatin1String("Folders"));
//    QStringList pendingInsertion = folderIds;

//    foreach (const QString &folderId, folderIds) {
//        KConfigGroup folderConfig(&foldersGroup, folderId);
//        QString parentId = folderConfig.readEntry("ParentId");

//        if (!pendingInsertion.contains(parentId)) {
//            addFolderItem(folderId,
//                          parentId,
//                          folderConfig.readEntry("ChangeKey"),
//                          folderConfig.readEntry("DisplayName"));
//            pendingInsertion.removeOne(folderId);
//        }
//    }

//    initFolders(pendingInsertion);
}

void EwsFolderModel::sync()
{
    QSettings settings(m_configName, QSettings::NativeFormat);
    qDebug() << settings.fileName();
    settings.beginGroup(QLatin1String("SyncState"));
//    KConfig config(m_configName, KConfig::SimpleConfig);
//    KConfigGroup syncGroup(&config, QLatin1String("SyncState"));
    QString lastSyncState = settings.value("SyncState").toString();
    EwsSyncFolderHierarchyReply *reply = m_parent->connection()->syncFolderHierarch(EwsFolder::AllProperties,
                                                                                    QString(),
                                                                                    lastSyncState);
    connect(reply, SIGNAL(finished()), SLOT(syncFolderHierarchyFinished()));
}

void EwsFolderModel::syncFolderHierarchyFinished()
{
    EwsSyncFolderHierarchyReply *response = qobject_cast<EwsSyncFolderHierarchyReply*>(sender());
//    if (response->error()) {
//        qDebug() << Q_FUNC_INFO << "SyncFolderHierarchyReply failed" << response->errorMessage();
//        response->deleteLater();
//        return;
//    }

    QSettings settings(m_configName, QSettings::NativeFormat);
    settings.beginGroup(QLatin1String("SyncState"));
//    KConfig config(m_configName, KConfig::SimpleConfig);
//    KConfigGroup syncGroup(&config, QLatin1String("SyncState"));
    QString lastSyncState = settings.value("SyncState").toString();

    foreach (const EwsFolder &folder, response->createFolders()) {
        addFolder(folder);
        emit syncItems(folder.id());
    }

    foreach (const EwsFolder &folder, response->updateFolders()) {
        addFolder(folder);
    }

    foreach (const QString &folderId, response->deleteFolders()) {
        deleteFolder(folderId);
    }

    if (lastSyncState != response->syncState()) {
        settings.setValue("SyncState", response->syncState());
    }

    response->deleteLater();
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

void EwsFolderModel::addFolder(const EwsFolder &folder)
{
//    KConfig config(m_configName, KConfig::SimpleConfig);
//    KConfigGroup folders(&config, QLatin1String("Folders"));
//    bool itemChanged = false;
//    bool itemNew = true;
//    KConfigGroup folderConfig(&folders, folder.id());

//    if (folderConfig.isValid()) {
//        itemChanged = folderConfig.readEntry("ChangeKey") != folder.changeKey();
//        itemNew = false;
//    }

//    if (itemChanged || itemNew) {
//        folderConfig.writeEntry("ChangeKey", folder.changeKey());
//        folderConfig.writeEntry("ParentId", folder.parentId());
//        folderConfig.writeEntry("DisplayName", folder.displayName());

//        addFolderItem(folder.id(), folder.parentId(), folder.changeKey(), folder.displayName());
//    }
}

void EwsFolderModel::deleteFolder(const QString &folderId)
{
//    KConfig config(m_configName, KConfig::SimpleConfig);
//    KConfigGroup folders(&config, QLatin1String("Folders"));
//    KConfigGroup folderConfig(&folders, folderId);
//    folderConfig.deleteGroup();

//    QStandardItem *stdItem = findItem(folderId);
//    if (stdItem) {
//        kDebug() << stdItem->row() << stdItem->parent()->index();
//        removeRow(stdItem->row(), stdItem->parent()->index());
//    }
}

void EwsFolderModel::addFolderItem(const QString &id, const QString &parentId, const QString &changeKey, const QString &title)
{
    QStandardItem *stdItem = findItem(id);
    if (stdItem) {
        if (stdItem->data(RoleFolderParentId).toString() == parentId) {
            stdItem->setText(title);
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
    stdItem->setText(title);
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
