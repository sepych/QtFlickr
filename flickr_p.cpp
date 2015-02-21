/******************************************************************************
 *   Copyright (C) 2009 by Evgeni Gordejev                                    *
 *   evgeni.gordejev@gmail.com                                                *
 *   Copyright (C) 2015 by Jacob Dawid                                        *
 *   jacob@omg-it.works                                                       *
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

// Own includes
#include "flickr_p.h"
#include "flickr.h"

// Qt includes
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QCryptographicHash>

FlickrPrivate::FlickrPrivate(Flickr *parent) :
    _networkAccessManager(new QNetworkAccessManager(this)),
    _p(parent),
    _requestCounter(0) {
    connect(_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

FlickrPrivate::~FlickrPrivate() {
}


QUrl FlickrPrivate::authorizationUrl(const QString &frob,
                                     const QString &perms) {
    QString authUrl;
    QString apiSig = _apiSecret
            + "api_key"
            + _apiKey
            + "frob"
            + frob
            + "perms"
            + perms;

    apiSig = md5(apiSig);
    authUrl = "http://flickr.com/services/auth/?api_key="
            + _apiKey
            + "&perms="
            + perms
            + "&frob="
            + frob
            + "&api_sig="
            + apiSig;

    return QUrl(authUrl);
}

int FlickrPrivate::upload(const FlickrPhoto &photo,
                          const FlickrRequest &request,
                          void* userData) {
    QByteArray boundary = generateBoundary();
    QByteArray payload;
    QDataStream dataStream(&payload, QIODevice::WriteOnly);

    QMap<QString,QString> map = photo.args;

    map.insert("api_key", _apiKey);
    if(!_token.isEmpty()) {
        map.insert("auth_token", _token);
    }

    bool uploading = photo.photoId.isEmpty();
    if(!uploading){
        map.insert("photo_id", photo.photoId);
    }

    QMapIterator<QString, QString> i(map);
    QStringList keyList;
    while(i.hasNext())
    {
        i.next();
        keyList << i.key();
    }
    qSort(keyList.begin(), keyList.end());

    QString apiSig(_apiSecret);
    for(int i = 0; i < keyList.size(); ++i) {
        apiSig.append(keyList.at(i) + map.value(keyList.at(i)));

        QByteArray field = constructField(keyList.at(i),map.value(keyList.at(i)),boundary);
        dataStream.writeRawData(field.data(), field.length());

    }

    apiSig = md5(apiSig);

    QByteArray sigField = constructField("api_sig", apiSig, boundary);
    dataStream.writeRawData(sigField.data(), sigField.length());

    QByteArray fileField = constructField("photo", "", boundary, photo.file);
    dataStream.writeRawData(fileField.data(), fileField.length());

    QFile file(photo.file);
    file.open(QIODevice::ReadOnly);
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        dataStream.writeRawData(line.data(),line.length());
    }

    file.close();

    QByteArray endField;
    endField.append("\r\n--");
    endField.append(boundary);
    endField.append("--\r\n\r\n");
    dataStream.writeRawData(endField.data(), endField.length());

    QString urlTmp("http://api.flickr.com/services/");
    urlTmp.append((uploading)?"upload/":"replace/");

    QNetworkRequest uploadRequest(urlTmp);
    uploadRequest.setRawHeader("Content-Type","multipart/form-data; boundary="+boundary);
    uploadRequest.setRawHeader("Host","ww.api.flickr.com");

    _requestCounter++;
    RequestData requestData;
    requestData.request = request.requests;
    requestData.userData = userData;
    requestData.requestId = _requestCounter;

    QNetworkReply *reply = _networkAccessManager->post(uploadRequest,payload);
    connect(reply,SIGNAL(uploadProgress(qint64, qint64)),
            this, SLOT(uploadProgress(qint64, qint64)));

    requestDataMap.insert(reply,requestData);
    return requestData.requestId;
}

void FlickrPrivate::replyFinished(QNetworkReply *reply) {
    // Network access manager might be shared
    // -> ignore requests from other users of the network access manager
    if (!requestDataMap.contains(reply)) {
        return;
    }

    QByteArray data = reply->readAll();
    qDebug() << "Flickr response: " << data;

    _flickrResponse.tags.clear();
    _flickrError.code = 0;
    _flickrError.message = "No Errors";

    if(reply->error() != QNetworkReply::NoError) {
        _flickrError.code = 1001;
        _flickrError.message = reply->errorString ();
    } else {
        parse(data, "rsp" , requestDataMap.value(reply).request);
    }

    void* userData = requestDataMap.value(reply).userData;
    int replyId = requestDataMap.value(reply).requestId;

    requestDataMap.remove(reply);
    emit _p->requestFinished(replyId,
                             _flickrResponse,
                             _flickrError,
                             userData);
}

void FlickrPrivate::uploadProgress(qint64 bytesSent, qint64 bytesTotal) {
    int percent;
    if(bytesTotal != -1) {
        percent = qRound(( qreal) bytesSent/(qreal) bytesTotal*100);
    } else {
        percent = -1;
    }

    emit _p->uploadProgress(percent);
}

QByteArray FlickrPrivate::generateBoundary() {
    const int lenght = 15;
    QChar unicode[lenght];
    for(int i = 0; i < lenght; ++i) {
        int sel = qrand() % 2;
        int temp = (sel) ? qrand() % 9 + 49 : qrand() % 23 + 98;
        unicode[i] = QChar(temp);
    }

    int size = sizeof(unicode) / sizeof(QChar);
    QString str = QString::fromRawData(unicode, size);

    return str.toLatin1();
}

QByteArray FlickrPrivate::constructField(QString name,
                                         QString content,
                                         QByteArray boundary,
                                         QString filename) {
    QByteArray data;
    data.append("--");
    data.append(boundary);
    data.append("\r\n");
    data.append("Content-Disposition: form-data; name=\"");
    data.append(name);
    if(filename.isEmpty()) {
        data.append("\"\r\n\r\n");
        data.append(content);
        data.append("\r\n");
    } else {
        data.append("\"; filename=\"");
        data.append(filename);
        data.append("\"\r\n");
        data.append("Content-Type: image/jpeg\r\n\r\n");
    }

    return data;
}

int FlickrPrivate::request(const FlickrMethod &method,
                           const FlickrRequest &request,
                           bool get,
                           void* userData) {
    QMap<QString,QString> map = method.args;
    map.insert("method", method.method);
    map.insert("api_key",_apiKey);
    if(!_token.isEmpty()) {
        map.insert("auth_token", _token);
    }

    QMapIterator<QString, QString> i(map);
    QStringList keyList;
    while(i.hasNext()) {
        i.next();
        keyList << i.key();
    }
    qSort(keyList.begin(), keyList.end());

    QString apiSig(_apiSecret);

    QUrl url = QUrl("https://www.flickr.com/services/rest/");

    QUrlQuery urlQuery;
    for(int i = 0; i < keyList.size(); ++i) {
        apiSig.append(keyList.at(i) + map.value(keyList.at(i)));
        urlQuery.addQueryItem(keyList.at(i),  map.value(keyList.at(i)));
    }

    urlQuery.addQueryItem("api_sig", md5(apiSig));
    url.setQuery(urlQuery);

    _requestCounter++;
    RequestData requestData;
    requestData.request = request.requests;
    requestData.userData = userData;
    requestData.requestId = _requestCounter;

    QNetworkReply *reply;
    if(!get) {
        reply = _networkAccessManager->post(QNetworkRequest(QUrl("https://www.flickr.com/services/rest/")),
                                            url.toEncoded(QUrl::RemoveFragment));
    } else {
        reply = _networkAccessManager->get(QNetworkRequest(url));
    }

    requestDataMap.insert(reply,requestData);
    return requestData.requestId;
}

void FlickrPrivate::parse(const QByteArray &data, const QString &startTag, const QMap<QString,QString> &request) {
    _xmlStreamReader.clear();
    _xmlStreamReader.addData(data);

    while(!_xmlStreamReader.atEnd()) {
        _xmlStreamReader.readNext();
        if(_xmlStreamReader.isStartElement()) {
            if(_xmlStreamReader.name() == startTag) {
                if(_xmlStreamReader.attributes().value("stat") == "ok") {
                    readElements(request);
                } else {
                    readErrorElement();
                }
            } else {
                _xmlStreamReader.raiseError(QObject::tr("The file is not an RSP file."));
            }
        }
    }

    if(_xmlStreamReader.hasError()) {
        _flickrError.code = 1002;
        _flickrError.message = _xmlStreamReader.errorString();
    }
}

void FlickrPrivate::readUnknownElement() {
    while(!_xmlStreamReader.atEnd()) {
        _xmlStreamReader.readNext();

        if(_xmlStreamReader.isEndElement()) {
            break;
        }

        if(_xmlStreamReader.isStartElement()) {
            readUnknownElement();
        }
    }
}

void FlickrPrivate::readElements(const QMap<QString,QString> &request) {
    while(!_xmlStreamReader.atEnd()) {
        _xmlStreamReader.readNext();

        if(_xmlStreamReader.isStartElement()) {
            readData(request);
        }
    }
}

void FlickrPrivate::readErrorElement() {
    while(!_xmlStreamReader.atEnd()) {
        _xmlStreamReader.readNext();

        if(_xmlStreamReader.isEndElement()) {
            break;
        }

        if(_xmlStreamReader.isStartElement()) {
            if(_xmlStreamReader.name() == "err") {
                readError();
            } else {
                readUnknownElement();
            }
        }
    }
}

void FlickrPrivate::readError() {
    bool ok;
    _flickrError.code = _xmlStreamReader.attributes().value("code").toString().toInt(&ok);
    _flickrError.message = _xmlStreamReader.attributes().value("msg").toString();
    _xmlStreamReader.readElementText();
}

void FlickrPrivate::readData(const QMap<QString,QString> &request) {
    QMap<QString, QString>::const_iterator i = request.find(_xmlStreamReader.name().toString());
    if(i != request.end()) {
        FlickrTag tagData;

        QList<QString> attrKeys = i.value().split(",");
        for(int j = 0; j < attrKeys.size(); ++j) {
            QString value = _xmlStreamReader.attributes().value(attrKeys.at(j)).toString();
            tagData.attrs.insert(attrKeys.at(j), value);
        }

        if(_xmlStreamReader.readNext() == QXmlStreamReader::Characters) {
            tagData.value =  _xmlStreamReader.text ().toString();
        }
        _flickrResponse.tags.insertMulti(i.key(),tagData);
    }
}

QString FlickrPrivate::md5(const QString &data) {
    QString hash(QCryptographicHash::hash(data.toLatin1(),QCryptographicHash::Md5).toHex());
    return hash;
}
