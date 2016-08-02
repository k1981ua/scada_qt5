#-------------------------------------------------
#
# Project created by QtCreator 2011-09-06T16:00:59
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = scada_qt5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mymodbus.cpp \
    tag.cpp \
    alarm.cpp \
    trendlogger.cpp \
    odbcdb.cpp \
    logger.cpp \
    usercode.cpp

HEADERS  += mainwindow.h \
    mymodbus.h \
    tag.h \
    alarm.h \
    trendlogger.h \
    odbcdb.h \
    logger.h \
    usercode.h \
    autostopthread.h

FORMS    += mainwindow.ui \
    logger.ui

RESOURCES += \
    res.qrc




























