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

#include "AccountNew.h"
#include "AccountsEngine.h"

#include <EwsQt5/autodiscover.h>

#include <QPushButton>
#include <QUuid>

#include <QSettings>
#include <QDebug>

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

using namespace Ews;

AccountNew::AccountNew(QObject *parent)
{
    __uid_t uid = geteuid();
    passwd *pwd = getpwuid(uid);
    if (pwd) {
        QString gecos = QString::fromLocal8Bit(pwd->pw_gecos);
        m_fullName = gecos.section(QLatin1Char(','), 0, 0);
    }
}

AccountNew::~AccountNew()
{
}

void AccountNew::setServerAddress(const QString &serverAddress)
{
    m_serverAddress = serverAddress;
    m_serverAddressIsModified = true;
    emit serverAddressChanged();
}

void AccountNew::setUsername(const QString &username)
{
    m_username = username;
    m_usernameIsModified = true;
    emit usernameChanged();
}

void AccountNew::setEmailAddress(const QString &emailAddress)
{
    if (!m_usernameIsModified) {
        m_username = emailAddress.section(QLatin1Char('@'), 0, 0);
        emit usernameChanged();
    }

    if (!m_serverAddressIsModified) {
        m_serverAddress = emailAddress.section(QLatin1Char('@'), 1, 1);
        emit serverAddressChanged();
    }

    m_emailAddress = emailAddress;
    m_description = m_serverAddress.section(QLatin1Char('.'), 0, 0);
    emit emailAddressChanged();
}

void AccountNew::process()
{
    if (m_processing) {
        return;
    }

    m_processing = true;
    emit processingChanged();

    AutoDiscover *autodiscover = new AutoDiscover(this);
    connect(autodiscover, &AutoDiscover::finished, this, &AccountNew::autoDiscoverFinished);
    autodiscover->autodiscover(m_emailAddress, m_serverAddress, m_username, m_password);
}

void AccountNew::save()
{
    QSettings settings;
    settings.beginGroup("Accounts");
    bool ok = false;
    for (int i = 0; i < 10; ++i) {
        QUuid uuid = QUuid::createUuid();
        QString uuidString;
        uuidString = uuid.toString().remove(QLatin1Char('}')).remove(QLatin1Char('{'));
        if (!settings.childKeys().contains(uuidString)) {
            settings.beginGroup(uuidString);
            QVariantHash::ConstIterator it = m_settings.constBegin();
            while (it != m_settings.constEnd()) {
                settings.setValue(it.key(), it.value());
                ++it;
            }
            settings.setValue("Fullname", m_fullName);
            settings.setValue("Description", m_description);
            settings.setValue("Kind", "EWS");
            settings.endGroup();
            ok = true;
            break;
        }
    }
    settings.endGroup();

    AccountsEngine::instance()->configFileChanged();
}

void AccountNew::autoDiscoverFinished()
{
    AutoDiscover *autodiscover = qobject_cast<AutoDiscover*>(sender());
    if (autodiscover && autodiscover->isValid()) {
        m_settings["EmailAddress"] = autodiscover->emailAddress();
        m_settings["URI"] = autodiscover->uri().toString();
        m_settings["internalASUrl"] = autodiscover->internalASUrl();
        m_settings["externalASUrl"] = autodiscover->externalASUrl();

        if (autodiscover->uri().host() != m_serverAddress) {
            m_serverAddress = autodiscover->uri().host();
        }

        m_valid = true;
        emit validChanged();
        emit setupServer();
    } else if (autodiscover && autodiscover->authRequired()) {
        emit authenticationError(autodiscover->uri().host());
    } else if (autodiscover) {
        qWarning() << Q_FUNC_INFO << autodiscover->errorMessage();
        emit setupServer();
    } else {
        emit setupServer();
    }

    m_processing = false;
    emit processingChanged();
}
