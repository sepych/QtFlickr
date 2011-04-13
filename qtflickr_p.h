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

#ifndef QTFLICKR_P_H
#define QTFLICKR_P_H

#include <QtCore>
#include <QObject>
#include <QXmlStreamReader>
#include "qtflickr.h"

class QtFlickr;
class QNetworkAccessManager;
class QNetworkReply;



class QtFlickrPrivate : public QObject
{
    Q_OBJECT
public:
    QtFlickrPrivate( QtFlickr *parent , QNetworkAccessManager* nam = NULL );
    ~QtFlickrPrivate();
    QString token;
    QString apiKey;
    QString apiSecret;
    QUrl authorizationUrl ( const QString &frob, const QString &perms );
    int request ( const QtfMethod &method, const QtfRequest &request, bool get, void* userData );
    int upload ( const QtfPhoto &photo, const QtfRequest &request, void* userData );
private slots:
    void replyFinished ( QNetworkReply *reply );
    void uploadProgress ( qint64 bytesSent, qint64 bytesTotal );
private:
    QByteArray generateBoundary();
    QByteArray constructField ( QString name,
                                QString content,
                                QByteArray boundary,
                                QString filename = QString() );

    void parse ( const QByteArray &data, const QString &startTag,
                 const QMap<QString,QString> &request );
    void readUnknownElement();
    void readElements ( const QMap<QString,QString> &request );
    void readErrorElement( );
    void readError();
    void readData( const QMap<QString,QString> &request );

    struct RequestData
    {
        void* userData;
        QMap<QString,QString> request;
        int requestId;
    };

    QString md5 ( const QString &data );

    QNetworkAccessManager *manager;
    QXmlStreamReader xml;

    QtfResponse response;
    QMap<QNetworkReply*,RequestData> requestDataMap;

    QtfError err;
    QtFlickr *p_ptr;
    int requestCounter;
};

#endif // QTFLICKR_P_H
