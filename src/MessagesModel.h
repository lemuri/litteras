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

#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include <QStandardItemModel>

class MessagesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum MessageRoles {
        RoleMessageId = Qt::UserRole + 1,
        RoleParentFolderId
    };
    explicit MessagesModel(QObject *parent = 0);

public slots:
    void addMessage(const QString &id, const QString &parentId, const QString &subject, const QString &from, const QDateTime &received, bool read);

private:
    QStandardItem *findItem(const QString &id);
};

#endif // MESSAGESMODEL_H
