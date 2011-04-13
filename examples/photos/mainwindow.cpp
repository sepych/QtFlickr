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
#include "photo.h"
#include <QtGui>

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

    photoView = new PhotoView();
    connect ( photoView,SIGNAL ( giveMeMore(int) ),this,SLOT ( getPhotos(int) ) );
    this->setCentralWidget ( photoView );

    getPhotos(MAX_PHOTOS);
}


MainWindow::~MainWindow()
{
}

void MainWindow::getPhotos(int num)
{
    QtfMethod method;
    method.method = "flickr.photos.getRecent";
    method.args.insert( "per_page", QString().setNum(num) );
    method.args.insert( "extras", "url_s, description" );
    QtfRequest request;
    request.requests.insert( "photo", "url_s, description" );
    request.requests.insert( "description", "" );
    qtFlickr->get ( method,request );
}

void MainWindow::requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData )
{
    if ( !err.code )
    {

        QList<QtfTag> list = data.tags.values("photo");
        QList<QtfTag> listD = data.tags.values("description");
        for ( int i = 0; i < list.size(); ++i )
        {
            photoView->addPhoto ( list.at ( i ).attrs.value("url_s") );
            qDebug()<<list.at ( i ).attrs.value("url_s");
            qDebug()<<listD.at ( i ).value;
        }

    }
    else
    {
        qDebug() <<err.message;
    }
}
