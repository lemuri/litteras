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

#ifndef EWSENGINE_H
#define EWSENGINE_H

#include <QObject>
#include <QQueue>
#include <QSettings>

class EwsFolderModel;
class EwsConnection;
class EwsSyncFolderItemsReply;
class EwsEngine : public QObject
{
    Q_OBJECT
public:
    explicit EwsEngine(const QSettings &config, const QString &uuid, QObject *parent = 0);

    EwsFolderModel *folderModel() const;
    EwsConnection *connection() const;
    QString uuid() const;

signals:
    void addMessage(const QString &id, const QString &parentId, const QString &subject, const QString &from, const QDateTime &received, bool read);

private slots:
    void update();
    void syncItems(const QString &folderId);
    void syncFolderItemsFinished();

private:
    QString m_uuid;
    EwsConnection *m_connection;
    EwsFolderModel *m_folderModel;
};

#endif // EWSENGINE_H
