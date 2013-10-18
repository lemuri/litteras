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

#ifndef ACCOUNTSENGINE_H
#define ACCOUNTSENGINE_H

#include <QObject>
#include <QAbstractItemModel>

class EwsEngine; //TODO create an abstract engine
class AccountsEngine : public QObject
{
    Q_OBJECT
public:
    explicit AccountsEngine(QObject *parent = 0);

    QList<QAbstractItemModel*> engineFolderModels();

signals:
    void addMessage(const QString &id, const QString &parentId, const QString &subject, const QString &from, const QDateTime &received, bool read);

private slots:
    void configFileChanged();

private:
    QHash<QString, EwsEngine*> m_accounts;
};

#endif // ACCOUNTSENGINE_H
