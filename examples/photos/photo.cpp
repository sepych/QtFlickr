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

#include "photo.h"
#include <QPaintEvent>
#include <QPainter>
#include <QNetworkAccessManager>
#include <QtCore>

#define WIDGET_SIZE_HALF 120
#define WIDGET_SIZE WIDGET_SIZE_HALF*2
#define SLIDESHOW_DURATION 7000
#define FADE_DURATION 1000

PhotoView::PhotoView ( QWidget * parent )
    : QWidget ( parent ),
    index ( 0 )
{
    //timer used to change photos
    timer = new QTimer ( this );
    connect ( timer, SIGNAL ( timeout() ), this, SLOT ( nextPhoto() ) );
    timer->start ( SLIDESHOW_DURATION );

    //timeline used for fading effect
    timeLine = new QTimeLine(FADE_DURATION, this);
    timeLine->setFrameRange(0, 100);
    timeLine->setCurveShape(QTimeLine::LinearCurve);
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(setValue(int)));

    opacity = 1.0;
    this->setFixedSize ( WIDGET_SIZE,WIDGET_SIZE );
}


PhotoView::~PhotoView()
{
}


void PhotoView::paintEvent ( QPaintEvent * event )
{
    //painting widget content
    QPainter painter ( this );
    painter.fillRect ( event->rect(),QColor ( 200,200,200 ) );
    if ( !imageList.isEmpty() )
    {
        painter.setOpacity(opacity);
        QSize imageSize = imageList.at ( index ).size();

        //adjusting photo to the center
        int x = WIDGET_SIZE_HALF - (imageSize.width()/2);
        int y = WIDGET_SIZE_HALF - (imageSize.height()/2);
        painter.drawImage ( x,y,imageList.at ( index ) );
    }
}

void PhotoView::addPhoto ( const QString &url )
{
    //setting photo into request queue
    QNetworkAccessManager *manager = new QNetworkAccessManager ( this );
    connect ( manager, SIGNAL ( finished ( QNetworkReply* ) ),
              this, SLOT ( replyFinished ( QNetworkReply* ) ) );

    manager->get ( QNetworkRequest ( QUrl ( url ) ) );
}

void PhotoView::replyFinished ( QNetworkReply * reply )
{
    QImage image;
    //getting image from received binary data
    if ( image.loadFromData ( reply->readAll() ) )
    {
        imageList.append ( image );
    }
    update();
}

void PhotoView::setValue(int i)
{
    //change photo in the middle of fading animation
    if(i > 50 && changePhoto){
        index++;
        changePhoto = !changePhoto;
    }

    //fadeout <--> fadein
    if(i < 50)
        opacity = (50.0-(qreal)i)/50.0;
    else
        opacity = ((qreal)i-50.0)/50.0;

    update();
}

void PhotoView::nextPhoto()
{
    if ( imageList.isEmpty() )
        return;

    //rolling slideshow to beginning if it reaches the end
    if ( index == imageList.size() )
        index = 0;

    //slideshow is in the middle
    if( index == (MAX_PHOTOS-CACHE_PHOTOS)){
        //remove already showed photos
        while(imageList.size() > (MAX_PHOTOS-CACHE_PHOTOS)){
            imageList.removeFirst();
        }
        //get 5 new photos
        emit giveMeMore(CACHE_PHOTOS);
        index = 0;
    }

    changePhoto = true;
    timeLine->start();
}
