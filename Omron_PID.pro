#-------------------------------------------------
#
# Project created by QtCreator 2017-09-06T17:03:49
#
#-------------------------------------------------

QT       += core gui serialport serialbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Omron_PID
TEMPLATE = app

win32:RC_ICONS += icon.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    configuredialog.cpp \
    joindialog.cpp \
        main.cpp \
        mainwindow.cpp \
    mythread.cpp \
    plotdialog.cpp \
    qcustomplot.cpp \
    tempdropdialog.cpp

HEADERS += \
    configuredialog.h \
    joindialog.h \
        mainwindow.h \
    mythread.h \
    plotdialog.h \
    qcustomplot.h \
    tempdropdialog.h

FORMS += \
        mainwindow.ui

DISTFILES += \
    icon.ico

RESOURCES += \
    resource.qrc


OPENSSL_DIR=C:/Qt/Tools/OpenSSL/Win_x64
LIBS += $${OPENSSL_DIR}/lib/libcrypto.lib
LIBS += $${OPENSSL_DIR}/lib/libssl.lib
