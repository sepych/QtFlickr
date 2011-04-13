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

#include "qtflickr_p.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QCryptographicHash>
#include "qtflickr.h"

QtFlickrPrivate::QtFlickrPrivate( QtFlickr *parent, QNetworkAccessManager *nam )
    : p_ptr(parent),
    requestCounter(0)
{
    if (nam) {
        manager = nam;
    } else {
        manager = new QNetworkAccessManager ( this );
    }
    connect ( manager, SIGNAL ( finished ( QNetworkReply* ) ),
              this, SLOT ( replyFinished ( QNetworkReply* ) ) );
}

QtFlickrPrivate::~QtFlickrPrivate()
{

}


QUrl QtFlickrPrivate::authorizationUrl ( const QString &frob, const QString &perms )
{
    QString authUrl;
    QString apiSig = apiSecret+"api_key"+apiKey+"frob"+frob+"perms"+perms;

    apiSig = md5 ( apiSig );
    authUrl = "http://flickr.com/services/auth/?api_key="+apiKey+
              "&perms="+perms+"&frob="+frob+"&api_sig="+apiSig;

    return QUrl ( authUrl );
}


int QtFlickrPrivate::upload ( const QtfPhoto &photo,
                              const QtfRequest &request,
                              void* userData )
{
    QByteArray boundary = generateBoundary();
    QByteArray payload;
    QDataStream dataStream ( &payload, QIODevice::WriteOnly );

    QMap<QString,QString> map = photo.args;


    map.insert ( "api_key", apiKey );
    if ( !token.isEmpty() )
    {
        map.insert ( "auth_token", token );
    }

    bool uploading = photo.photoId.isEmpty();
    if(!uploading){
        map.insert ( "photo_id", photo.photoId );
    }

    QMapIterator<QString, QString> i ( map );
    QStringList keyList;
    while ( i.hasNext() )
    {
        i.next();
        keyList << i.key();
    }
    qSort ( keyList.begin(), keyList.end() );

    QString apiSig ( apiSecret );
    for ( int i = 0; i < keyList.size(); ++i )
    {
        apiSig.append ( keyList.at ( i ) + map.value ( keyList.at ( i ) ) );

        QByteArray field = constructField ( keyList.at ( i ),map.value ( keyList.at ( i ) ),boundary );
        dataStream.writeRawData ( field.data(), field.length() );

    }
    apiSig = md5 ( apiSig );

    QByteArray sigField = constructField ( "api_sig", apiSig, boundary );
    dataStream.writeRawData ( sigField.data(), sigField.length() );

    QByteArray fileField = constructField ( "photo", "", boundary, photo.file );
    dataStream.writeRawData ( fileField.data(), fileField.length() );

    QFile file ( photo.file );
    file.open ( QIODevice::ReadOnly );
    while ( !file.atEnd() )
    {
        QByteArray line = file.readLine();
        dataStream.writeRawData ( line.data(),line.length() );
    }

    file.close();

    QByteArray endField;
    endField.append ( "\r\n--" );
    endField.append ( boundary );
    endField.append ( "--\r\n\r\n" );
    dataStream.writeRawData ( endField.data(), endField.length() );

    QString urlTmp("http://api.flickr.com/services/");
    urlTmp.append((uploading)?"upload/":"replace/");

    QNetworkRequest uploadRequest ( urlTmp );
    uploadRequest.setRawHeader ( "Content-Type","multipart/form-data; boundary="+boundary );
    uploadRequest.setRawHeader ( "Host","ww.api.flickr.com" );

    requestCounter++;
    RequestData requestData;
    requestData.request = request.requests;
    requestData.userData = userData;
    requestData.requestId = requestCounter;

    QNetworkReply *reply = manager->post ( uploadRequest,payload );
    connect(reply,SIGNAL(uploadProgress ( qint64, qint64 )),
            this, SLOT(uploadProgress ( qint64, qint64 )));

    requestDataMap.insert ( reply,requestData );
    return requestData.requestId;
}


/************************* PRIVATE SLOTS **************************/
/******************************************************************/
void QtFlickrPrivate::replyFinished ( QNetworkReply *reply )
{
    // Network access manager might be shared
    // -> ignore requests from other users of the network access manager
    if (!requestDataMap.contains(reply)) {
        return;
    }

    QByteArray data = reply->readAll();
    qDebug()<<"*******************************RESPONSE*******************************";
    qDebug()<<data;
    qDebug()<<"**********************************************************************\n\n";

    response.tags.clear();
    err.code = 0;
    err.message = "No Errors";

    if ( reply->error() != QNetworkReply::NoError )
    {
        err.code = 1001;
        err.message = reply->errorString ();
    }
    else
    {
        parse ( data, "rsp" , requestDataMap.value ( reply ).request );
    }

    void* userData = requestDataMap.value ( reply ).userData;
    int replyId = requestDataMap.value ( reply ).requestId;

    requestDataMap.remove ( reply );
    emit p_ptr->requestFinished ( replyId, response, err, userData );
}

void QtFlickrPrivate::uploadProgress ( qint64 bytesSent, qint64 bytesTotal )
{
    int percent;
    if ( bytesTotal != -1 )
        percent = qRound ( ( qreal ) bytesSent/ ( qreal ) bytesTotal*100 );
    else
        percent = -1;

    emit p_ptr->uploadProgress ( percent );
}

/********************* PRIVATE FUNCTIONS **************************/
/******************************************************************/
QByteArray QtFlickrPrivate::generateBoundary()
{
    const int lenght = 15;
    QChar unicode[lenght];
    for ( int i = 0; i < lenght; ++i )
    {
        int sel = qrand() % 2;
        int temp = ( sel ) ? qrand() % 9 + 49 : qrand() % 23 + 98;
        unicode[i] = QChar ( temp );
    }

    int size = sizeof ( unicode ) / sizeof ( QChar );
    QString str = QString::fromRawData ( unicode, size );

    return str.toLatin1();
}

QByteArray QtFlickrPrivate::constructField ( QString name,
                                             QString content,
                                             QByteArray boundary,
                                             QString filename )
{
    QByteArray data;
    data.append ( "--" );
    data.append ( boundary );
    data.append ( "\r\n" );
    data.append ( "Content-Disposition: form-data; name=\"" );
    data.append ( name );
    if ( filename.isEmpty() )
    {
        data.append ( "\"\r\n\r\n" );
        data.append ( content );
        data.append ( "\r\n" );
    }
    else
    {
        data.append ( "\"; filename=\"" );
        data.append ( filename );
        data.append ( "\"\r\n" );
        data.append ( "Content-Type: image/jpeg\r\n\r\n" );
    }

    return data;
}


int QtFlickrPrivate::request ( const QtfMethod &method, const QtfRequest &request, bool get, void* userData )
{
    QMap<QString,QString> map = method.args;
    map.insert("method", method.method);
    map.insert ( "api_key",apiKey );
    if ( !token.isEmpty() )
    {
        map.insert ( "auth_token", token );
    }

    QMapIterator<QString, QString> i ( map );
    QStringList keyList;
    while ( i.hasNext() )
    {
        i.next();
        keyList << i.key();
    }
    qSort ( keyList.begin(), keyList.end() );

    QString apiSig ( apiSecret );
    QUrl url ( "http://www.flickr.com/services/rest/", QUrl::TolerantMode );
    for ( int i = 0; i < keyList.size(); ++i )
    {
        apiSig.append ( keyList.at ( i ) + map.value ( keyList.at ( i ) ) );
        url.addQueryItem ( keyList.at ( i ),  map.value ( keyList.at ( i ) ) );
    }

    url.addQueryItem ( "api_sig",  md5 ( apiSig ) );

    requestCounter++;
    RequestData requestData;
    requestData.request = request.requests;
    requestData.userData = userData;
    requestData.requestId = requestCounter;

    QNetworkReply *reply;
    if ( !get )
        reply = manager->post ( QNetworkRequest ( QUrl("http://www.flickr.com/services/rest/") ), url.encodedQuery () );
    else
        reply = manager->get ( QNetworkRequest ( url ) );

    requestDataMap.insert ( reply,requestData );
    return requestData.requestId;
}

void QtFlickrPrivate::parse ( const QByteArray &data, const QString &startTag, const QMap<QString,QString> &request )
{
    xml.clear();
    xml.addData ( data );

    while ( !xml.atEnd() )
    {
        xml.readNext();
        if ( xml.isStartElement() )
        {
            if ( xml.name() == startTag )
            {
                if ( xml.attributes().value ( "stat" ) == "ok" )
                    readElements ( request );
                else
                    readErrorElement();
            }
            else
            {
                xml.raiseError ( QObject::tr ( "The file is not an RSP file." ) );
            }
        }
    }

    if ( xml.hasError() )
    {
        err.code = 1002;
        err.message = xml.errorString();
    }
}

void QtFlickrPrivate::readUnknownElement()
{
    while ( !xml.atEnd() )
    {
        xml.readNext();

        if ( xml.isEndElement() )
            break;

        if ( xml.isStartElement() )
            readUnknownElement();
    }
}

void QtFlickrPrivate::readElements ( const QMap<QString,QString> &request )
{
    while ( !xml.atEnd() )
    {
        xml.readNext();

        if ( xml.isStartElement() )
        {
            readData(request);
        }
    }
}


void QtFlickrPrivate::readErrorElement( )
{
    while ( !xml.atEnd() )
    {
        xml.readNext();

        if ( xml.isEndElement() )
            break;

        if ( xml.isStartElement() )
        {
            if ( xml.name() == "err" )
                readError();
            else
                readUnknownElement();
        }
    }
}

void QtFlickrPrivate::readError()
{
    bool ok;
    err.code = xml.attributes().value ( "code" ).toString().toInt ( &ok );
    err.message = xml.attributes().value ( "msg" ).toString();
    xml.readElementText();
}


void QtFlickrPrivate::readData(const QMap<QString,QString> &request)
{
    QMap<QString, QString>::const_iterator i = request.find ( xml.name().toString() );
    if ( i != request.end() )
    {
        QtfTag tagData;

        QList<QString> attrKeys = i.value().split(",");
        for ( int j = 0; j < attrKeys.size(); ++j )
        {
            QString value = xml.attributes().value ( attrKeys.at ( j ) ).toString();
            tagData.attrs.insert ( attrKeys.at ( j ), value );
        }

        if ( xml.readNext() == QXmlStreamReader::Characters )
        {
            tagData.value =  xml.text ().toString();
        }
        response.tags.insertMulti ( i.key(),tagData );
    }
}

QString QtFlickrPrivate::md5 ( const QString &data )
{
    QString hash ( QCryptographicHash::hash ( data.toLatin1(),QCryptographicHash::Md5 ).toHex() );
    return hash;
}
