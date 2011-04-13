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
{
    label = new QLabel();
    this->setCentralWidget(label);


    qtFlickr = new QtFlickr("xxx", "xxx", this);
    connect(qtFlickr,SIGNAL(requestFinished ( int, QtfResponse, QtfError, void* )),
            this,SLOT(requestFinished ( int, QtfResponse, QtfError, void* )));

    QSettings settings("QtFlickr","Authorization");
    QString token = settings.value("token").toString();
    if(!token.isEmpty()){
        qtFlickr->setToken(token);

        QString username = settings.value("username").toString();
        QString greetings("Welcome, ");
        greetings.append(username);
        label->setText(greetings);
    }else{
        QtfMethod method;
        method.method = "flickr.auth.getFrob";
        QtfRequest request;
        request.requests.insert("frob","");
        requestId[GetFrob] = qtFlickr->get(method,request);
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData )
{
    if(!err.code){
        if(reqId == requestId[GetFrob]){
            QString frob = data.tags.value("frob").value;
            QUrl authUrl = qtFlickr->authorizationUrl(frob);
            QDesktopServices::openUrl ( authUrl );

            QMessageBox msgBox;
            msgBox.setText("Press Ok button when you have completed authorization through web browser");
            int result = msgBox.exec();
            if( result == QMessageBox::Ok){
                QtfMethod method;
                method.method = "flickr.auth.getToken";
                method.args.insert( "frob", frob );
                QtfRequest request;
                request.requests.insert("token","");
                request.requests.insert("user","username,fullname");
                requestId[GetToken] = qtFlickr->get(method, request);
            }
        }else if(reqId == requestId[GetToken]){
            QString token = data.tags.value("token").value;
            QString username = data.tags.value("user").attrs.value("username");
            QString fullname = data.tags.value("user").attrs.value("fullname");

            qtFlickr->setToken(token);

            QSettings settings("QtFlickr","Authorization");
            settings.setValue("username",username);
            settings.setValue("token",token);

            QString greetings("Welcome, ");
            greetings.append(username);
            label->setText(greetings);
            //Now you can call authorized calls with "write" permission
        }
    }else{
        qDebug()<<"Error: "<<err.message;
    }
}

