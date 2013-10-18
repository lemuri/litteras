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

#include "MainUI.h"
#include "ui_MainUI.h"

#include "NewMail.h"
#include "AccountNew.h"
#include "Accounts.h"
#include "AccountsEngine.h"
#include "MessagesModel.h"
#include "NoSelectionRectDelegate.h"

#include <QDebug>

MainUI::MainUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainUI)
{
    ui->setupUi(this);

    ui->toolBar->addAction(QIcon::fromTheme("dialog-cancel"), "New email", this, SLOT(newMail()));
//    ui->toolBar->addAction("Reply",this, SLOT(reply()));
//    ui->toolBar->addAction("Forward", this, SLOT(forward()));
//    ui->toolBar->addAction("Send/Receive", this, SLOT(sendReceive()));
//    ui->toolBar->addAction("Trash", this, SLOT(trash()));
//    ui->toolBar->addAction("Print", this, SLOT(print()));

     QWidget *foldersViewport = ui->foldersView->viewport();
     QPalette palette = foldersViewport->palette();
     palette.setColor(foldersViewport->backgroundRole(), Qt::transparent);
     foldersViewport->setPalette(palette);
     ui->foldersView->setItemDelegate(new NoSelectionRectDelegate(this));

     m_messagesModel = new MessagesModel(this);
     ui->messagesView->setModel(m_messagesModel);
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::setEngine(AccountsEngine *engine)
{
    if (engine->engineFolderModels().size()) {
        ui->foldersView->setModel(engine->engineFolderModels().first());
    }
    connect(engine, SIGNAL(addMessage(QString,QString,QString,QString,QDateTime,bool)),
            m_messagesModel, SLOT(addMessage(QString,QString,QString,QString,QDateTime,bool)));
}

void MainUI::newMail()
{
    NewMail *dialog = new NewMail;
    dialog->show();
}

void MainUI::on_actionNewAccount_triggered()
{
    AccountNew *dialog = new AccountNew;
    dialog->show();
}

void MainUI::on_actionAccounts_triggered()
{
    Accounts *dialog = new Accounts;
    dialog->show();
}
