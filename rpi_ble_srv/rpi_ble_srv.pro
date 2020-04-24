QT -= gui
QT += bluetooth

CONFIG += c++14 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
        uuid.hpp \
        connection_handler.hpp \
        device_info.hpp
SOURCES += \
        main.cpp \
        connection_handler.cpp \
        device_info.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
