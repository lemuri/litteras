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

#ifndef ACCOUNTNEW_H
#define ACCOUNTNEW_H

#include <QObject>
#include <QVariantHash>

namespace Ews {
class AutoDiscover;
}

class AccountNew : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverAddress MEMBER m_serverAddress NOTIFY serverAddressChanged)
    Q_PROPERTY(QString username MEMBER m_username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString emailAddress MEMBER m_emailAddress WRITE setEmailAddress NOTIFY emailAddressChanged)
    Q_PROPERTY(QString password MEMBER m_password NOTIFY passwordChanged)
    Q_PROPERTY(QString fullName MEMBER m_fullName NOTIFY fullNameChanged)
    Q_PROPERTY(QString description MEMBER m_description NOTIFY descriptionChanged)
    Q_PROPERTY(bool hideUsername MEMBER m_hideUsername NOTIFY hideUsernameChanged)
    Q_PROPERTY(bool processing MEMBER m_processing NOTIFY processingChanged)
    Q_PROPERTY(bool valid MEMBER m_valid NOTIFY validChanged)
public:
    explicit AccountNew(QObject *parent = 0);
    ~AccountNew();

    void setUsername(const QString &username);
    void setEmailAddress(const QString &emailAddress);
    void setDescription(const QString &description);

signals:
    void serverAddressChanged();
    void usernameChanged();
    void emailAddressChanged();
    void passwordChanged();
    void fullNameChanged();
    void descriptionChanged();
    void hideUsernameChanged();
    void processingChanged();
    void validChanged();
    void authenticationError(const QString &server);
    void setupServer();

public slots:
    void process();
    void save();
    void cancel();

private:
    void autoDiscoverFinished();

    Ews::AutoDiscover *m_autodiscover = 0;
    QString m_serverAddress;
    QString m_username;
    QString m_emailAddress;
    QString m_password;
    QString m_fullName;
    QString m_description;
    bool m_hideUsername = true;
    bool m_processing = false;
    bool m_valid = false;
    bool m_usernameIsModified = false;

    QVariantHash m_settings;
};

#endif // ACCOUNTNEW_H
