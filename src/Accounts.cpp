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

#include "Accounts.h"
#include "ui_Accounts.h"

#include <QSettings>
#include <QDebug>

Accounts::Accounts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Accounts),
    m_model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->listView->setModel(m_model);

    initModel();
}

Accounts::~Accounts()
{
    delete ui;
}

void Accounts::initModel()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("Accounts"));
    qDebug() << Q_FUNC_INFO << settings.childGroups();
    foreach (const QString &group, settings.childGroups()) {
        settings.beginGroup(group);
        QStandardItem *item = new QStandardItem;
        item->setText(settings.value("EmailAddress").toString());
        item->setData(settings.value("URI").toString());
        m_model->appendRow(item);
        settings.endGroup();
    }
}
