/***************************************************************************
 *   Copyright (C) 2008-2011 by Daniel Nicoletti                           *
 *   dantti12@gmail.com                                                    *
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
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#include "Litteras.h"
#include <config.h>

#include "EwsFolderModel.h"
#include "foldermodel.h"
#include "MessagesModel.h"

#include <QApplication>

#include <QQmlApplicationEngine>
#include <qqml.h>

#include <QDebug>

#include "AccountsEngine.h"
#include "AccountNew.h"

static QObject *account_engine_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return AccountsEngine::instance();
}

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(application);

    QCoreApplication::setOrganizationName("lemuri");
    QCoreApplication::setOrganizationDomain("lemuri.org");
    QCoreApplication::setApplicationName("Litteras");
    QCoreApplication::setApplicationVersion(APP_VERSION);

    QApplication app(argc, argv);

    qmlRegisterSingletonType<AccountsEngine>("org.lemuri.litteras", 1, 0, "AccountsEngine", account_engine_provider);
    qmlRegisterType<FolderModel>("org.lemuri.litteras", 1, 0, "FolderModel");
    qmlRegisterType<MessagesModel>("org.lemuri.litteras", 1, 0, "MessagesModel");
    qmlRegisterType<AccountNew>("org.lemuri.litteras", 1, 0, "AccountNew");

    QQmlApplicationEngine *engine = new QQmlApplicationEngine(QUrl(QLatin1String("qrc:/qml/main.qml")));

    return app.exec();
}
