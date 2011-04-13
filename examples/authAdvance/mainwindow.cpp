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
#include <QWebView>
#include <QScrollArea>

MainWindow::MainWindow()
{
    view = new QWebView();
    msg = new QLabel("Press Ok button when you have completed program authorization.");
    okBut = new QPushButton("Ok");
    connect(okBut,SIGNAL(clicked()),this,SLOT(getToken()));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(msg);
    hLayout->addWidget(okBut);
    hLayout->addStretch();
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(view);

    QWidget *main = new QWidget();
    main->setLayout(vLayout);
    this->setCentralWidget(main);

    qtFlickr = new QtFlickr("xxx", "xxx", this);
    connect(qtFlickr,SIGNAL(requestFinished ( int, QtfResponse, QtfError, void* )),
            this,SLOT(requestFinished ( int, QtfResponse, QtfError, void* )));

    QSettings settings("QtFlickr","Authorization");
    //uncomment next line to "logout"
    //settings.clear();
    QString token = settings.value("token").toString();
    if(!token.isEmpty()){
        qtFlickr->setToken(token);
        greetings(settings.value("username").toString());
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

void MainWindow::getToken()
{
    QtfMethod method;
    method.method = "flickr.auth.getToken";
    method.args.insert( "frob", frob );
    QtfRequest request;
    request.requests.insert("token","");
    request.requests.insert("user","username,fullname");
    requestId[GetToken] = qtFlickr->get(method, request);
}

void MainWindow::requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData )
{
    if(!err.code){
        if(reqId == requestId[GetFrob]){
            frob = data.tags.value("frob").value;
            QUrl authUrl = qtFlickr->authorizationUrl(frob);
            view->setUrl(authUrl);
        }else if(reqId == requestId[GetToken]){
            QString token = data.tags.value("token").value;
            QString username = data.tags.value("user").attrs.value("username");
            QString fullname = data.tags.value("user").attrs.value("fullname");

            qtFlickr->setToken(token);

            QSettings settings("QtFlickr","Authorization");
            settings.setValue("username",username);
            settings.setValue("token",token);

            greetings(username);
            //Now you can call authorized calls with "write" permission
        }
    }else{
        qDebug()<<"Error: "<<err.message;
    }
}

void MainWindow::greetings(const QString &username)
{
    msg->hide();
    okBut->hide();
    QString htmlStr = "<html><body>"
                      "<i>Welcome, "+username+
                      "</i></body></html>";
    view->setHtml(htmlStr);
}

QLabel* MainWindow::linuxThumbs(const QString &uri)
{
    QPixmap xThumb;
    QByteArray uriForHash("file://");
    uriForHash.append(uri);

    QByteArray thumbUri(QDir::homePath().toLatin1());
    //maemo uses cropped folder to store thumbnails
    thumbUri.append("/.thumbnails/normal/");
    thumbUri.append(QCryptographicHash::hash(uriForHash,QCryptographicHash::Md5).toHex());
    //maemo uses .jpeg
    thumbUri.append(".png");

    if(!xThumb.load(thumbUri)){
        xThumb.load(uri);
    }

    xThumb = xThumb.scaled(124,124,Qt::KeepAspectRatioByExpanding);
    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(xThumb);
    return iconLabel;
}

