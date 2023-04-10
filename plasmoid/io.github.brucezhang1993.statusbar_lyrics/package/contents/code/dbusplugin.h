#ifndef _PLUGINS_H_
#define _PLUGINS_H_

#include <QtCore/QtGlobal>
#include <QtQml/QQmlExtensionPlugin>

class DBusPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
#if !defined(STATIC_PLUGIN_DBUS)
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
#endif

public:
    explicit DBusPlugin(QObject *parent = 0);
    void registerTypes(const char *uri);
};

#endif /* _PLUGINS_H_ */
