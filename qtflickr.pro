TEMPLATE = lib
CONFIG += \
    static

QT += \
    network \
    xml

VERSION = 0.1.0

TARGET = qtflickr

HEADERS += \
    flickr.h \
    flickr_p.h \
    flickrerror.h \
    flickrmethod.h \
    flickrrequest.h \
    flickrphoto.h \
    flickrtag.h \
    flickrresponse.h

SOURCES += \
    flickr.cpp \
    flickr_p.cpp
