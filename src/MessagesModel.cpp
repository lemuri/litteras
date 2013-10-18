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

#include "MessagesModel.h"

MessagesModel::MessagesModel(QObject *parent) :
    QStandardItemModel(parent)
{
    QStringList headers;
    headers << QLatin1String("From");
    headers << QLatin1String("Subject");
    headers << QLatin1String("Received");
    setHorizontalHeaderLabels(headers);
}

void MessagesModel::addMessage(const QString &id, const QString &parentId, const QString &subject, const QString &from, const QDateTime &received, bool read)
{
    QStandardItem *stdItem = findItem(id);
    if (stdItem) {
        return;
    }

    QList<QStandardItem*> items;
    stdItem = new QStandardItem;
    QFont font = stdItem->font();
    font.setBold(true);
    if (!read) {
        stdItem->setFont(font);
    }
    stdItem->setText(from);
    stdItem->setData(id, RoleMessageId);
    stdItem->setData(parentId, RoleParentFolderId);
    items << stdItem;

    stdItem = new QStandardItem(subject);
    if (!read) {
        stdItem->setFont(font);
    }
    items << stdItem;

    stdItem = new QStandardItem;
    if (!read) {
        stdItem->setFont(font);
    }
    stdItem->setData(received, Qt::DisplayRole);
    items << stdItem;

    appendRow(items);
}

QStandardItem *MessagesModel::findItem(const QString &id)
{
    for (int i = 0; i < rowCount(); ++i) {
        QModelIndex item = index(i, 0);
        if (item.data(RoleMessageId).toString() == id) {
            return itemFromIndex(item);
        }
    }

    return 0;
}
