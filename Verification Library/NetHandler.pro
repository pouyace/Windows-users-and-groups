QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TEMPLATE = lib
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    nethandler.cpp

HEADERS += \
    nethandler.h


unix|win32: LIBS += -lMPR
unix|win32: LIBS += -lNetapi32
# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32 {
    DESTDIR     += $$OUT_PWD/../Dest
}

#include($$OTMLibPath/OTMPriFunctions.pri)
SharedLibsPath = $$(OTMSharedLibs)
isEmpty(SharedLibsPath){
    error("NetHandler not found");
}
LibPath = $$SharedLibsPath/NetHandler

equals(TEMPLATE,"lib"){
    CONFIG(debug,debug|release):TARGET = $${TARGET}d
    DESTDIR = $$LibPath/bin
    bN = $$basename(_PRO_FILE_PWD_)
    neededHeaders.path = $$LibPath/inc
    neededHeaders.files = $$_PRO_FILE_PWD_/*.h
    INSTALLS *= neededHeaders
}
include(OTMPriFunctions.pri)
placeInstalls();
