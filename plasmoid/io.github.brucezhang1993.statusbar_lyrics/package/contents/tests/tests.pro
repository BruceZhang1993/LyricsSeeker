TEMPLATE=app
TARGET=tst_dbus
CONFIG += warn_on qmltestcase
SOURCES += tst_dbus.cpp
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"

QT += qml dbus

OTHER_FILES += \
    auto/*
