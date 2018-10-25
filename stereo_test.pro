#-------------------------------------------------
#
# Project created by QtCreator 2017-08-26T13:04:37
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stereo_test
TEMPLATE = app
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    stereovision.cpp \
    settingsdialog.cpp \
    disparitydialog.cpp \
    graphicsview.cpp \
    qcustomplot.cpp \
    plotterdialog.cpp

HEADERS  += mainwindow.h \
    stereovision.h \
    wrapper.h \
    settingsdialog.h \
    disparitydialog.h \
    graphicsview.h \
    qcustomplot.h \
    plotterdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    disparitydialog.ui \
    plotterdialog.ui

INCLUDEPATH += -I/usr/local/include/opencv2
LIBS += -L/usr/local/include/opencv2 -lopencv_core -lopencv_highgui -lopencv_imgproc
#QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS_DEBUG += -O
QMAKE_CXXFLAGS_RELEASE += -O3
#LIBS += -lgomp
