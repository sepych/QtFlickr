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
 * @struct FlickrRequest
 * Use this structure to provide information for the XML parser, parser will
 * search only those tags that you have been provided. Request is map container,
 * where key is a tag name and value is a comma delimited list of possible attributes.<br>
 * Attributes are optional, provide only empty string for tag only request.
 * Example:
 * @code
 * <?xml version="1.0" encoding="utf-8" ?>
 * <rsp stat="ok">
 * 	<photos page="2" pages="89" perpage="10" total="881">
 * 		<photo id="2636" owner="47058503995@N01"/>
 * 		<photo id="2635" owner="47058503995@N01"/>
 * 	</photos>
 * </rsp>
 * @endcode
 * Let assume server response is a XML above and you want to get all id's and owner information for the
 * photo list. The code will look like this:
 * @code
 * FlickrRequest request;
 * request.requests.insert("photo","id,owner");
 *
 * FlickrMethod method;
 * method.method = "flickr.photos.getWithGeoData";
 *
 * Flickr qtFlickr("apiKey","apiSecret");
 * qtFlickr.get(method, request);
 * @endcode
 */
struct FlickrRequest {
    QMap<QString,QString> requests;

    /** Default constructor */
    FlickrRequest() { }

    /** Constructs and initialize single request */
    FlickrRequest(const QString &tag,
                  const QString &attrs = QString()) {
        requests.insert(tag,attrs);
    }
};
