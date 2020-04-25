QT += core gui widgets bluetooth svg

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

ios {
    QMAKE_INFO_PLIST = $$PWD/Info.plist
}

SOURCES += \
    banner_view.cpp \
    devicefinder.cpp \
    devicehandler.cpp \
    deviceview.cpp \
    spinner.cpp \
    main.cpp

HEADERS += \
    banner_view.h \
    devicefinder.h \
    devicehandler.h \
    deviceview.h \
    spinner.h \
    uuid.hpp

RESOURCES = resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
