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
 * @struct QtfPhoto
 * This structure is used when uploading or replacing photos.
 * @param file File to be uploaded
 * @param photoId The ID of the photo to replace, do not set this
 * value if you want just to upload photo
 * @param args Optional arguments, for example "title"
 * Example:
 * @code
 * QtfPhoto photo;
 * photo.file = "/home/user/picture.png";
 * photo.args.insert("title", "Greatest pic of all time!");
 *
 * QtFlickr qtFlickr("apiKey","apiSecret");
 * //uploading requires authentication
 * ... (missing code here)
 *
 * qtFlickr.upload(photo);
 * @endcode
 */
struct FlickrPhoto {
    QString file;
    QString photoId;
    QMap<QString,QString> args;

    /** Default constructor */
    FlickrPhoto() { }

    /** Constucts and initialize photo photo structure */
    FlickrPhoto(const QString &file,
                const QString &photoId = QString()) :
        file(file),
        photoId(photoId) {
    }
};
