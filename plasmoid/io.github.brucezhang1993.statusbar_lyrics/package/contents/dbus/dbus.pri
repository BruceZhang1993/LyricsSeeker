# Include this file in your .pro file to statically compile this Plugin
# plugin into your project.
#
# Basic usage instructions:
#
#  #include <plugin.h>
#
#  int main(int argc, char *argv[])
#  {
#      QApplication app(argc, argv);
#
#      Plugin plugin;
#      plugin.registerTypes("DBus");
#
#      ...
#  }

INCLUDEPATH += $$PWD

HEADERS += \
        $$PWD/declarativedbusadaptor.h \
        $$PWD/declarativedbusinterface.h \
    $$PWD/dbusplugin.h

SOURCES += \
        $$PWD/declarativedbusadaptor.cpp \
        $$PWD/declarativedbusinterface.cpp \
    $$PWD/dbusplugin.cpp
