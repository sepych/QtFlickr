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
#include <QtWebKit>

MainWindow::MainWindow()
    : QMainWindow()
{
    // Initialising flickr class with valid apikey and apisecret
    qtFlickr = new QtFlickr ( "8239143606e389cd1b5200e78ad9d9a1",
                              "907ed90a816eea46",
                              this );

    // connecting listner to the requestFinished() signal
    connect ( qtFlickr,
              SIGNAL ( requestFinished ( int, QtfResponse, QtfError, void* ) ),
              this,
              SLOT ( requestFinished ( int, QtfResponse, QtfError, void* ) ) );

    //local url is needed for qwebview object
    localUrl =  QUrl::fromLocalFile ( QDir::currentPath()+"/style.css" );

    this->constructUi();
}


MainWindow::~MainWindow()
{
}

void MainWindow::constructUi()
{
    // ui layout
    respView = new QWebView();
    //respView->setMinimumSize(600,800);
    respView->setHtml(constructHtml(), localUrl);

    connect(respView->page()->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,
            SLOT(populateJavaScriptWindowObject()));

    this->setCentralWidget ( respView );
}

void MainWindow::findUser(const QString &name)
{
    if(name.isEmpty())
        return;

    // Searching user
    QtfMethod method;
    method.method = "flickr.people.findByUsername";
    method.args.insert("username", name);

    QtfRequest request;
    request.requests.insert("user","id,nsid");
    request.requests.insert("username","");
    requestId[GetUser] = qtFlickr->get(method,request);
}

void MainWindow::requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData )
{
    if ( !err.code )
    {
        if(requestId[GetUser] == reqId){
            QString userid ( data.tags.value("user").attrs.value("id") );

            //request for user photos
            QtfMethod method("flickr.people.getPublicPhotos");
            method.args.insert("user_id",userid);
            method.args.insert("extras","url_s");
            method.args.insert("per_page","6");
            QtfRequest request("photo","url_s");
            requestId[GetPhotos] = qtFlickr->get(method,request);
        }else if(requestId[GetPhotos] == reqId){
            QList<QtfTag> photos = data.tags.values("photo");
            //place photos into webview
            respView->setHtml(constructHtml(photos),localUrl);
        }
    }
}

QString MainWindow::constructHtml(const QList<QtfTag> &photos)
{
    QString header = "<html><head><link href='style.css' rel='stylesheet' type='text/css' media='screen' />"
                     "<script type='text/javascript' src='script.js'></script> "
                     "</head><body><div class='header'><h3>Find photos by username</h3><form onsubmit='extractName()'>"
                     "<input type='text'' id='name'><input type='submit' value='Find'></form></div><div class='content'>";

    QString footer = "</div></body></html>";


    QString body = "<table><tr>";
    int col = 2;
    for(int i = 0; i < photos.size(); ++i){
        col--;
        body.append("<td>");
        body.append("<img src='");
        body.append(photos.at(i).attrs.value("url_s"));
        body.append("'></td>");
        if(!col){
            col = 2;
            body.append("</tr></tr>");
        }
    }
    body.append("</tr></table>");

    QString htmlStr;
    htmlStr.append(header);
    htmlStr.append(body);
    htmlStr.append(footer);
    return htmlStr;
}

void MainWindow::populateJavaScriptWindowObject()
{
    respView->page()->mainFrame()->addToJavaScriptWindowObject("htmlView", this);
}
