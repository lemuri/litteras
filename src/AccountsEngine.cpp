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

//#include <KConfig>
//#include <KConfigGroup>
//#include <KStandardDirs>
//#include <KDirWatch>

//#include <KDebug>

AccountsEngine::AccountsEngine(QObject *parent) :
    QObject(parent)
{
//    KDirWatch *confWatch = new KDirWatch(this);
//    confWatch->addFile(KStandardDirs::locateLocal("config", "litterasrc"));
//    connect(confWatch, SIGNAL(dirty(QString)), SLOT(configFileChanged()));
//    connect(confWatch, SIGNAL(created(QString)), SLOT(configFileChanged()));
//    connect(confWatch, SIGNAL(deleted(QString)), SLOT(configFileChanged()));
//    confWatch->startScan();

    configFileChanged();
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
        connect(engine, SIGNAL(addMessage(QString,QString,QString,QString,QDateTime,bool)),
                this, SIGNAL(addMessage(QString,QString,QString,QString,QDateTime,bool)));
        settings.endGroup();
    }
//    kDebug();
//    KConfig config;
//    KConfigGroup accounts(&config, QLatin1String("Accounts"));
//    foreach (const QString &group, accounts.groupList()) {
//        if (m_accounts.contains(group)) {
//            continue;
//        }

//        KConfigGroup configGroup(&accounts, group);
//        EwsEngine *engine = new EwsEngine(configGroup, this);
//        connect(engine, SIGNAL(addMessage(QString,QString,QString,QString,QDateTime,bool)),
//                this, SIGNAL(addMessage(QString,QString,QString,QString,QDateTime,bool)));
//        m_accounts.insert(group, engine);
//    }
}
