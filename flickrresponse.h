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
#include "flickrtag.h"

// Qt includes
#include <QMultiMap>

/**
 * @struct FlickrResponse
 * Structure holds data from XML parser.
 * it will includes only those values that has been setted with the QtfRequest structure<br>
 * Example:
 * @code
 * <?xml version="1.0" encoding="utf-8" ?>
 * <rsp stat="ok">
 * 	<photos page="2" pages="89" perpage="10" total="881">
 * 		<photo id="2636" owner="47058503995@N01"/>
 * 		<photo id="2635" owner="47058503995@N01"/>
 * 	</photos>
 *      <name>cat</name>
 * </rsp>
 * @endcode
 * If you want to get page number and name from XML above:
 * @code
 * QString page = response.tags.value("photos").attrs.value("page");
 * qDebug()<<page //output: 2
 * QString name = response.tags.value("name").value;
 * qDebug()<<name //output: cat
 * @endcode
 */
struct FlickrResponse {
    QMultiMap<QString,FlickrTag> tags;
};
