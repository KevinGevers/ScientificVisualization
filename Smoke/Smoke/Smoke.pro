#-------------------------------------------------
#
# Project created by QtCreator 2019-02-23T15:06:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Smoke
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        smokewindow.cpp \
    simulation.cpp \
    visualization.cpp \
    cone.cpp \
    cylinder.cpp

HEADERS += \
        smokewindow.h \
    simulation.h \
    visualization.h \
    cone.h \
    cylinder.h

FORMS += \
        smokewindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# OpenGL
win32 {
    LIBS += -lopengl32 -lglu32
}

# FFTW3
win32 {
    LIBS += -L$$PWD/../FFTW/ -lfftw3-3

    INCLUDEPATH += $$PWD/../FFTW
    DEPENDPATH += $$PWD/../FFTW
} else {
    LIBS += -lfftw3
}
