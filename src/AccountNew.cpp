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

#include <EwsQt5/EwsAutoDiscover.h>

#include <QPushButton>
#include <QUuid>

//#include <KUser>
#include <QSettings>
#include <QDebug>

AccountNew::AccountNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AccountNew)
{
    ui->setupUi(this);
    connect(ui->emailAddressLE, SIGNAL(textChanged(QString)), SLOT(emailAddressChanged(QString)));
    ui->usernameL->hide();
    ui->usernameLE->hide();
    setModal(true);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(commit()));

//    KUser user;
//    ui->fullNameLE->setText(user.fullName());
}

AccountNew::~AccountNew()
{
    delete ui;
}

void AccountNew::slotButtonClicked(int button)
{
//    if (button == KDialog::Ok) {
//        enableButton(KDialog::Ok, false);
//        EwsAutoDiscover *autodiscover = new EwsAutoDiscover(this);
//        connect(autodiscover, SIGNAL(finished()), this, SLOT(autoDiscoverFinished()));
//        autodiscover->autodiscover(ui->emailAddressLE->text(), ui->usernameLE->text(), ui->passwordLE->text());
//    } else {
//        KDialog::slotButtonClicked(button);
    //    }
}

void AccountNew::commit()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    EwsAutoDiscover *autodiscover = new EwsAutoDiscover(this);
    connect(autodiscover, SIGNAL(finished()), this, SLOT(autoDiscoverFinished()));
    autodiscover->autodiscover(ui->emailAddressLE->text(), ui->usernameLE->text(), ui->passwordLE->text());
}

void AccountNew::autoDiscoverFinished()
{
    EwsAutoDiscover *autodiscover = qobject_cast<EwsAutoDiscover*>(sender());
    if (autodiscover) {
        qWarning() << Q_FUNC_INFO << autodiscover->errorMessage();
        if (autodiscover->isValid()) {
            QSettings settings;
            settings.beginGroup(QLatin1String("Accounts"));
//            KConfig config;
//            KConfigGroup accounts(&config, QLatin1String("Accounts"));
            bool ok = false;
            for (int i = 0; i < 10; ++i) {
                QUuid uuid = QUuid::createUuid();
                QString uuidString;
                uuidString = uuid.toString().remove(QLatin1Char('}')).remove(QLatin1Char('{'));
                if (!settings.childKeys().contains(uuidString)) {
//                    KConfigGroup group(&accounts, uuidString);
                    settings.beginGroup(uuidString);
                    settings.setValue("EmailAddress", autodiscover->emailAddress());
                    settings.setValue("URI", autodiscover->uri().toString());
                    settings.setValue("ASUrl", autodiscover->asUrl());
                    settings.setValue("OABUrl", autodiscover->oabUrl());
                    settings.endGroup();
                    ok = true;
                    break;
                }
            }
            settings.endGroup();

            if (ok) {
                accept();
            }

//            ui->messageWidget->setText(i18n("Failed to find an UUID for this account, please try again."));
//            ui->messageWidget->animatedShow();
        } else {
            qWarning() << Q_FUNC_INFO << autodiscover->errorMessage();
//            ui->messageWidget->setText(i18n("Failed to auto configure account:\n%1",
//                                            autodiscover->errorMessage()));
//            ui->messageWidget->animatedShow();
//            ui->usernameL->show();
//            ui->usernameLE->show();
//            enableButton(KDialog::Ok, true);
        }
    }
}

void AccountNew::emailAddressChanged(const QString &text)
{
    if (!ui->usernameLE->isModified()) {
        ui->usernameLE->setText(text.section(QLatin1Char('@'), 0, 0));
    }
}
