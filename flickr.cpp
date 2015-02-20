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
#include "flickr.h"
#include "flickr_p.h"

Flickr::Flickr(const QString &apiKey,
               const QString &apiSecret,
               QObject *parent,
               QNetworkAccessManager *nam) :
    QObject(parent),
    d( new FlickrPrivate( this, nam)) {
    d->_apiKey = apiKey;
    d->_apiSecret = apiSecret;
}


Flickr::~Flickr() {
    delete d;
}

void Flickr::setToken(const QString &token) {
    d->_token = token;
}

QUrl Flickr::authorizationUrl(const QString &frob, const QString &perms) {
    return d->authorizationUrl(frob, perms);
}

int Flickr::get(const FlickrMethod &method, const FlickrRequest &req, void* userData) {
    return d->request(method, req, true, userData);
}

int Flickr::post(const FlickrMethod &method, const FlickrRequest &req, void* userData) {
    return d->request(method, req, false, userData);
}

int Flickr::upload(const FlickrPhoto &photo, const FlickrRequest &req, void* userData) {
    return d->upload(photo, req, userData);
}
