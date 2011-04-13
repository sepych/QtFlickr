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

#ifndef QTFLICKR_H
#define QTFLICKR_H

#include <QtCore>

class QtFlickrPrivate;
class QNetworkAccessManager;

/**
 * @struct QtfError
 * Flickr errors. Find more about flickr errors at http://www.flickr.com/services/api/
 * error codes:<br>
 * <ul><li>0 - No errors</li>
 * </ul>
 */
struct QtfError{
    int code;
    QString message;
};


/**
 * @struct QtfMethod
 * Use this structure to create method for accessing Flickr API<br>
 * Example:
 * @code
 * QtfMethod method;
 * method.method = "flickr.photos.comments.addComment";
 * method.args.insert("photo_id","87243209893");
 * method.args.insert("comment_text","This comment was added through QtFlickr library :) ");
 *
 * QtFlickr qtFlickr("apiKey","apiSecret");
 * qtFlickr.get(method);
 * @endcode
 * <b><i>Note: You don't have to provide api_key as argument.</i></b>
 */
struct QtfMethod{
    QString method;
    QMap<QString,QString> args;

    /** Default constructor */
    QtfMethod(){}
    /** Construct and initialize method */
    QtfMethod(const QString &method):method(method){}
};


/**
 * @struct QtfRequest
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
 * QtfRequest request;
 * request.requests.insert("photo","id,owner");
 *
 * QtfMethod method;
 * method.method = "flickr.photos.getWithGeoData";
 *
 * QtFlickr qtFlickr("apiKey","apiSecret");
 * qtFlickr.get(method, request);
 * @endcode
 */
struct QtfRequest{
    QMap<QString,QString> requests;

    /** Default constructor */
    QtfRequest(){}
    /** Constructs and initialize single request */
    QtfRequest(const QString &tag, const QString &attrs = QString())
    {
        requests.insert(tag,attrs);
    }
};


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
struct QtfPhoto{
    QString file;
    QString photoId;
    QMap<QString,QString> args;

    /** Default constructor */
    QtfPhoto(){}
    /** Constucts and initialize photo photo structure */
    QtfPhoto(const QString &file, const QString &photoId = QString()):file(file),photoId(photoId){}
};


/**
  * @struct QtfTag
  * Holds response data, see QtfResponse
  */
struct QtfTag{
    QString value;
    QMap<QString,QString> attrs;
};


/**
 * @struct QtfResponse
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
struct QtfResponse{
    QMultiMap<QString,QtfTag> tags;
};


/**
 * @class QtFlickr
 * Main class of QtFlickr APi<br>
 * Authorization example:
 * @code
 * UIClass::UIClass
 * {
 * 	qtFlickr = new QtFlickr("xxxxxxxxx", "xxxxxxxxxxxxx", this);
 * 	connect(qtFlickr,SIGNAL(requestFinished ( int, QtfResponse, QtfError, void* )),
 * 		this,SLOT(requestFinished ( int, QtfResponse, QtfError, void* )));
 *
 * 	QtfMethod method;
 *      method.method = "flickr.auth.getFrob";
 *
 * 	QtfRequest request;
 *      request.requests.insert("frob","");
 * 	frobRequest = qtFlickr->get(method,request);
 * }
 *
 * void UIClass::requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData )
 * {
 * 	if(err.code != 0){
 * 		if(reqId == frobRequest){
 * 			QString frob = data.tags.value("frob").value;
 * 			QUrl authUrl = qtFlickr->authorizationUrl(frob);
 * 			QDesktopServices::openUrl ( authUrl );
 *
 * 			QMessageBox msgBox;
 * 			msgBox.setText("Press Ok button when you have completed authorization through web browser")
 * 			int result = msgBox.exec();
 * 			if( result == QDialog::Accepted){
 * 				QtfMethod method;
 *                              method.method = "flickr.auth.getToken";
 * 				method.args.insert( "frob", frob );
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
class QtFlickr : public QObject
{
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
    QtFlickr ( const QString &apiKey,
               const QString &apiSecret,
               QObject *parent = 0,
               QNetworkAccessManager *nam = 0 );

    ~QtFlickr();

    /**
     * Sets token for the QtFlickr API
     * @param token
     */
    void setToken ( const QString &token );

    /**
     * Returns authorization url for give frob
     * @param frob
     * @param perms "write", "read" or "delete"
     */
    QUrl authorizationUrl ( const QString &frob, const QString &perms = "write" );

    /**
     * Fetches data from Flickr API with GET method
     * @param method Flickr API method
     * @param request XML request data
     * @param userData user data to be transfered through signal/slots mechanism,
     * this parametr has no effect on this function
     * @return request id
     */
    int get ( const QtfMethod &method,
              const QtfRequest &request = QtfRequest(),
              void* userData = 0 );

    /**
     * Fetches data from Flickr API with POST method
     * @param method Flickr API method
     * @param request XML request data
     * @param userData user data to be transfered through signal/slots mechanism,
     * this parametr has no effect on this function
     * @return request id
     */
    int post ( const QtfMethod &method,
               const QtfRequest &request = QtfRequest(),
               void* userData = 0 );

    /**
     * Uploads or replaces photo
     * @param photo to be uploaded
     * @param request XML request data
     * @param userData user data to be transfered through signal/slots mechanism,
     * this parametr has no effect on this function
     * @return request id
     */
    int upload ( const QtfPhoto &photo,
                 const QtfRequest &request = QtfRequest(),
                 void* userData = 0 );

signals:
    /**
     * Emitted after get(), post() and upload() functions
     * @param reqId The request id
     * @param data Response XML data
     * @param err possible error
     * @param userData user data transfered through signal/slots mechanism
     */
    void requestFinished ( int reqId, QtfResponse response, QtfError err, void* userData );

    /**
     * Emitted while photo uploading is in progress
     * @param percent 0-100, returns -1 when cannot determine uploading
     * progress.
     */
    void uploadProgress ( int percent );

private:
    QtFlickrPrivate * const d;
    friend class QtFlickrPrivate;
};

#endif
