QT += core gui bluetooth svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

ios {
    QMAKE_INFO_PLIST = $$PWD/Info.plist
}

SOURCES += \
    devicefinder.cpp \
    devicehandler.cpp \
    deviceview.cpp \
    mainwindow.cpp \
    topbanner.cpp \
    main.cpp

HEADERS += \
    devicefinder.h \
    devicehandler.h \
    deviceview.h \
    mainwindow.h \
    topbanner.h \
    uuid.hpp

RESOURCES = resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
