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

#ifndef PHOTO_H
#define PHOTO_H

#include <QWidget>
#include <QNetworkReply>
#include <QTimer>
#include <QTimeLine>

#define MAX_PHOTOS 10
#define CACHE_PHOTOS 5

class PhotoView : public QWidget
{
    Q_OBJECT
public:
    PhotoView(QWidget * parent = 0);

    ~PhotoView();
    void addPhoto ( const QString &url );
signals:
    void giveMeMore(int num);
private slots:
    void replyFinished ( QNetworkReply * reply );
    void setValue(int i);
    void nextPhoto();
protected:
    void paintEvent ( QPaintEvent * event );
private:
    QList<QImage> imageList;
    int index;
    QTimer *timer;
    QTimeLine *timeLine;
    qreal opacity;
    bool changePhoto;
};

#endif
