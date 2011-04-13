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

#include "qtflickr.h"
#include "qtflickr_p.h"

QtFlickr::QtFlickr ( const QString &apiKey,
                     const QString &apiSecret,
                     QObject *parent )
                         :QObject ( parent ),
                         d(new QtFlickrPrivate(this))
{
    d->apiKey = apiKey;
    d->apiSecret = apiSecret;
}


QtFlickr::~QtFlickr()
{
    delete d;
}

void QtFlickr::setToken ( const QString &token )
{
    d->token = token;
}

QUrl QtFlickr::authorizationUrl ( const QString &frob, const QString &perms )
{
    return d->authorizationUrl(frob, perms);
}

int QtFlickr::get ( const QtfMethod &method, const QtfRequest &req, void* userData )
{
    return d->request ( method, req, true, userData );
}

int QtFlickr::post ( const QtfMethod &method, const QtfRequest &req, void* userData )
{
    return d->request ( method, req, false, userData );
}

int QtFlickr::upload ( const QtfPhoto &photo, const QtfRequest &req, void* userData )
{
    return d->upload(photo, req, userData);
}
