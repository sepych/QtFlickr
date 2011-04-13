/******************************************************************************
 *   Copyright (C) 2009 by Evgeni Gordejev   *
 *   evgeni.gordejev@gmail.com   *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Library Lesser General Public License as   *
 *   published by the Free Software Foundation; either version 2 of the       *
 *   License, or (at your option) any later version.                          *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Library Lesser General Public *
 *   License along with this program; if not, write to the                    *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include "mainwindow.h"
#include <QtGui>

MainWindow::MainWindow()
    : QMainWindow()
{
    // Initialising flickr class with valid apikey and apisecret
    qtFlickr = new QtFlickr ( "xxx",
                              "xxx",
                              this );

    // connecting listner to the requestFinished() signal
    connect ( qtFlickr,
              SIGNAL ( requestFinished ( int, QtfResponse, QtfError, void* ) ),
              this,
              SLOT ( requestFinished ( int, QtfResponse, QtfError, void* ) ) );

    this->constructUi();
}


MainWindow::~MainWindow()
{
}

void MainWindow::constructUi()
{
    // ui layout
    usernameEdit = new QLineEdit();
    respEdit = new QTextEdit();
    respEdit->setReadOnly ( true );

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow ( "Find User by Username:", usernameEdit );
    formLayout->addRow ( "Response:", respEdit );

    QWidget *main = new QWidget();
    main->setLayout ( formLayout );

    this->setCentralWidget ( main );

    connect ( usernameEdit,SIGNAL ( editingFinished () ),this,SLOT ( findUser() ) );
}

void MainWindow::findUser()
{
    // Searching user
    QtfMethod method;
    method.method = "flickr.people.findByUsername";
    method.args.insert("username", usernameEdit->text());

    QtfRequest request;
    request.requests.insert("user","id,nsid");
    request.requests.insert("username","");
    qtFlickr->get(method,request);
}

void MainWindow::requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData )
{
    if ( !err.code )
    {
        // printing user data on the screen
        QString userData ( "User id: "+data.tags.value("user").attrs.value("id")+"\n" );
        userData.append ( "User nsid: "+data.tags.value("user").attrs.value("nsid")+"\n" );
        userData.append ( "Username: "+data.tags.value("username").value+"\n" );
        respEdit->setPlainText ( userData );
    }
    else
    {
        respEdit->setPlainText ( err.message );
    }
}
