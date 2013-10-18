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

#include <QApplication>
#include <QDebug>
//#include <KConfig>
//#include <KAboutData>
//#include <KCmdLineArgs>

int main(int argc, char **argv)
{
//    KAboutData about("litteras",
//                     "litteras", // DO NOT change this catalog unless you know it will not break translations!
//                     ki18n("Litteras"),
//                     APP_VERSION,
//                     ki18n("Litteras is an Application to Get and Manage Software"),
//                     KAboutData::License_GPL,
//                     ki18n("(C) 2013 Daniel Nicoletti"));

//    about.addAuthor(ki18n("Daniel Nicoletti"), KLocalizedString(), "dantti12@gmail.com", "http://dantti.wordpress.com");
//    about.setProgramIconName("mail-flag");

//    KCmdLineArgs::init(argc, argv, &about);

//    KUniqueApplication::addCmdLineOptions();

    QCoreApplication::setOrganizationName("Pingloo");
    QCoreApplication::setOrganizationDomain("pingloo.org");
    QCoreApplication::setApplicationName("Litteras");
    QCoreApplication::setApplicationVersion("0.1");

    QApplication app(argc, argv);

    Litteras window;
    window.newInstance();

    return app.exec();
}
