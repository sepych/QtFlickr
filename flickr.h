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

#pragma once

// Own includes
#include "flickrerror.h"
#include "flickrmethod.h"
#include "flickrrequest.h"
#include "flickrphoto.h"
#include "flickrtag.h"
#include "flickrresponse.h"

// Qt includes
#include <QtCore>
class FlickrPrivate;
class QNetworkAccessManager;

/**
 * @class QtFlickr
 * Main class of QtFlickr APi<br>
 * Authorization example:
 * @code
 * UIClass::UIClass
 * {
 * 	qtFlickr = new QtFlickr("xxxxxxxxx", "xxxxxxxxxxxxx", this);
 * 	connect(qtFlickr,SIGNAL(requestFinished(int, QtfResponse, QtfError, void*)),
 * 		this,SLOT(requestFinished(int, QtfResponse, QtfError, void*)));
 *
 * 	QtfMethod method;
 *      method.method = "flickr.auth.getFrob";
 *
 * 	QtfRequest request;
 *      request.requests.insert("frob","");
 * 	frobRequest = qtFlickr->get(method,request);
 * }
 *
 * void UIClass::requestFinished(int reqId, QtfResponse data, QtfError err, void* userData)
 * {
 * 	if(err.code != 0){
 * 		if(reqId == frobRequest){
 * 			QString frob = data.tags.value("frob").value;
 * 			QUrl authUrl = qtFlickr->authorizationUrl(frob);
 * 			QDesktopServices::openUrl(authUrl);
 *
 * 			QMessageBox msgBox;
 * 			msgBox.setText("Press Ok button when you have completed authorization through web browser")
 * 			int result = msgBox.exec();
 * 			if(result == QDialog::Accepted){
 * 				QtfMethod method;
 *                              method.method = "flickr.auth.getToken";
 * 				method.args.insert("frob", frob);
 * 				QtfRequest request;
 *                              request.requests.insert("token","");
 * 				request.requests.insert("user","username,fullname");
 * 				tokenRequest = qtFlickr->get(method, request);
 * 			}
 * 		}else if(reqId == tokenRequest){
 * 			QString token = data.tags.value("token").value;
 * 			QString username = data.tags.value("user").attrs.value("username");
 * 			QString fullname = data.tags.value("user").attrs.value("fullname");
 *
 * 			qDebug()<<"Your username: "<<username;
 * 			qDebug()<<"Your fullname: "<<fullname;
 *
 * 			qtFlickr->setToken(token);
 *			//Now you can call authorized calls with "write" permission
 * 		}
 * 	}else{
 * 		qDebug()<<"Error: "<<err.message;
 * 	}
 * }
 * @endcode
 */
class Flickr : public QObject {
    friend class FlickrPrivate;
    Q_OBJECT
public:

    /**
     * Constructor
     * @param apiKey Flickr api key
     * @param apiSecret Flickr api secret
     * @param parent object
     * @param nam QNetworkAccessManager instance to use for networking requests
     *          (If nam is 0 a new instance is created)
     */
    Flickr(const QString &apiKey,
           const QString &apiSecret,
           QObject *parent = 0,
           QNetworkAccessManager *nam = 0);

    ~Flickr();

    /**
     * Sets token for the QtFlickr API
     * @param token
     */
    void setToken(const QString &token);

    /**
     * Returns authorization url for give frob
     * @param frob
     * @param perms "write", "read" or "delete"
     */
    QUrl authorizationUrl(const QString &frob, const QString &perms = "write");

    /**
     * Fetches data from Flickr API with GET method
     * @param method Flickr API method
     * @param request XML request data
     * @param userData user data to be transfered through signal/slots mechanism,
     * this parametr has no effect on this function
     * @return request id
     */
    int get(const FlickrMethod &method,
            const FlickrRequest &request = FlickrRequest(),
            void* userData = 0);

    /**
     * Fetches data from Flickr API with POST method
     * @param method Flickr API method
     * @param request XML request data
     * @param userData user data to be transfered through signal/slots mechanism,
     * this parametr has no effect on this function
     * @return request id
     */
    int post(const FlickrMethod &method,
             const FlickrRequest &request = FlickrRequest(),
             void* userData = 0);

    /**
     * Uploads or replaces photo
     * @param photo to be uploaded
     * @param request XML request data
     * @param userData user data to be transfered through signal/slots mechanism,
     * this parametr has no effect on this function
     * @return request id
     */
    int upload(const FlickrPhoto &photo,
               const FlickrRequest &request = FlickrRequest(),
               void* userData = 0);

signals:
    /**
     * Emitted after get(), post() and upload() functions
     * @param reqId The request id
     * @param data Response XML data
     * @param err possible error
     * @param userData user data transfered through signal/slots mechanism
     */
    void requestFinished(int reqId,
                         FlickrResponse response,
                         FlickrError err,
                         void* userData);

    /**
     * Emitted while photo uploading is in progress
     * @param percent 0-100, returns -1 when cannot determine uploading
     * progress.
     */
    void uploadProgress(int percent);

private:
    FlickrPrivate * const d;
};
