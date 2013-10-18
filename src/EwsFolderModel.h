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

#ifndef EWSFOLDERMODEL_H
#define EWSFOLDERMODEL_H

#include <QStandardItemModel>
#include <QTimer>

#include <EwsQt5/EwsFolder.h>

class EwsEngine;
class EwsFolderModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum FolderRoles {
        RoleFolderId = Qt::UserRole + 1,
        RoleFolderParentId,
        RoleChangeKey
    };
    explicit EwsFolderModel(EwsEngine *parent = 0);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QStringList folderIds(const QModelIndex &parent = QModelIndex()) const;

signals:
    void syncItems(const QString &folderId);

private slots:
    void init();
    void initFolders(const QStringList &folderIds);
    void sync();
    void syncFolderHierarchyFinished();
    void updateFolderFinished();

private:
    void addFolder(const EwsFolder &folder);
    void deleteFolder(const QString &folderId);
    void addFolderItem(const QString &id, const QString &parentId, const QString &changeKey, const QString &title);
    QStandardItem *findItem(const QString &id, const QModelIndex &parent = QModelIndex());

    EwsEngine *m_parent;
    QTimer *m_updateTimer;
    QString m_uuid;
    QString m_configName;
};

#endif // EWSFOLDERMODEL_H
