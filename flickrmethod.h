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

// Qt includes
#include <QString>
#include <QMap>

/**
 * @struct FlickrMethod
 * Use this structure to create method for accessing Flickr API<br>
 * Example:
 * @code
 * FlickrMethod method;
 * method.method = "flickr.photos.comments.addComment";
 * method.args.insert("photo_id","87243209893");
 * method.args.insert("comment_text","This comment was added through QtFlickr library :) ");
 *
 * Flickr qtFlickr("apiKey","apiSecret");
 * qtFlickr.get(method);
 * @endcode
 * <b><i>Note: You don't have to provide api_key as argument.</i></b>
 */
struct FlickrMethod {
    QString method;
    QMap<QString,QString> args;

    /** Default constructor */
    FlickrMethod() { }

    /** Construct and initialize method */
    FlickrMethod(const QString &method):method(method){}
};
