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
#include "ui_AccountNew.h"

#include <EwsQt5/autodiscover.h>

#include <QPushButton>
#include <QUuid>

//#include <KUser>
#include <QSettings>
#include <QDebug>

using namespace Ews;

AccountNew::AccountNew(QObject *parent)
{

}

AccountNew::~AccountNew()
{
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
    m_emailAddress = emailAddress;
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
    autodiscover->autodiscover(m_emailAddress, m_username, m_password);
    qDebug() << m_emailAddress << m_username << m_password;
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
            settings.endGroup();
            ok = true;
            break;
        }
    }
    settings.endGroup();
}

void AccountNew::autoDiscoverFinished()
{
    AutoDiscover *autodiscover = qobject_cast<AutoDiscover*>(sender());
    if (autodiscover && autodiscover->isValid()) {
        m_settings["EmailAddress"] = autodiscover->emailAddress();
        m_settings["URI"] = autodiscover->uri().toString();
        m_settings["ASUrl"] = autodiscover->asUrl();
        m_settings["OABUrl"] = autodiscover->oabUrl();

        m_valid = true;
        emit validChanged();
    } else if (autodiscover && autodiscover->authRequired()) {
        emit authenticationError();
    } else if (autodiscover) {
        qWarning() << Q_FUNC_INFO << autodiscover->errorMessage();
    }

    m_processing = false;
    emit processingChanged();
}
