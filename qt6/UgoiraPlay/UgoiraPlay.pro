QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    External/zlib/minizip/unzip.c \
    External/zlib/minizip/ioapi.c \
    main.cpp \
    ugoirafile.cpp \
    ugoiraview.cpp

HEADERS += \
    External/zlib/minizip/unzip.h \
    External/zlib/minizip/ioapi.h \
    ugoirafile.h \
    ugoiraview.h

FORMS += \
    ugoiraview.ui

unix|win32: LIBS += -L$$PWD/External/zlib/ -lzlib

INCLUDEPATH += $$PWD/External/zlib/includes
DEPENDPATH += $$PWD/External/zlib/includes

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/External/zlib/zlib.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/External/zlib/libzlib.a
