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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtflickr.h"

class QWebView;
class QLabel;
class QPushButton;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    enum FlickrRequest
    {
        GetFrob,
        GetToken,
        RequestCount
    };
private slots:
    void getToken();
    void requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData );

private:
    void greetings(const QString &username);
    //unused function
    QLabel* linuxThumbs(const QString &uri);

    QWebView *view;
    QLabel *msg;
    QPushButton *okBut;
    QtFlickr *qtFlickr;
    int requestId[RequestCount];

    QString frob;
};


#endif
