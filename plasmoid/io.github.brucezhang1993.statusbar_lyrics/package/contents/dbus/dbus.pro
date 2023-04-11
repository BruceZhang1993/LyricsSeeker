TEMPLATE = lib

TARGET = $$qtLibraryTarget(DBus)
TARGETPATH = DBus

QT += dbus qml
QT -= gui
CONFIG += plugin

DESTDIR = $$OUT_PWD/imports/DBus

PLUGIN_IMPORT_PATH = DBus

API_VER=1.0

contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols

DEPENDPATH += .
INCLUDEPATH += .

include(dbus.pri)

!isEmpty(QTPATH): target.path = $$QTPATH/imports/$$TARGETPATH
else: target.path = $$[QT_INSTALL_QML]/$$replace(TARGETPATH, \\., /).$$API_VER
;

importPath = $$[QT_INSTALL_QML]/$$replace(TARGETPATH, \\., /).$$API_VER
target.path = $${importPath}

QMAKE_POST_LINK = $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable DBus $$API_VER $$OUT_PWD/imports > $$OUT_PWD/imports/DBus/plugins.qmltypes

qmltypes.path = $$target.path
qmltypes.files += $$OUT_PWD/imports/DBus/plugins.qmltypes
export(qmltypes.files)

QMLFILES = $$PWD/qmldir

qmlplugin.commands = $$QMAKE_COPY $$QMLFILES $$OUT_PWD/imports/DBus/
qmlplugin.path = $$target.path

qmlpluginfiles.depends += qmlplugin
qmlpluginfiles.path = $$target.path
qmlpluginfiles.files = $$QMLFILES

first.depends += qmlplugin
export(first.depends)
export(qmlplugin.commands)

QMAKE_EXTRA_TARGETS += first qmlplugin
INSTALLS += target qmlpluginfiles qmltypes

