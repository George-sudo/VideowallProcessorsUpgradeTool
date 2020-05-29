#-------------------------------------------------
#
# Project created by QtCreator 2020-05-07T20:43:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VideowallProcessorsUpgradeTool
TEMPLATE = app

RC_FILE = logo.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    unzip_ufw.cpp \
    connection.cpp

HEADERS  += mainwindow.h \
    unzip_ufw.h \
    connection.h

FORMS    += mainwindow.ui \
    connection.ui

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/rapidjson

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/lib -lquazipd
} else {
    LIBS += -L$$PWD/lib -lquazip
}
