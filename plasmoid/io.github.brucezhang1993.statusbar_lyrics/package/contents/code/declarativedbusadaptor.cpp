/****************************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jollamobile.com>
** All rights reserved.
**
** You may use this file under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software Foundation
** and appearing in the file license.lgpl included in the packaging
** of this file.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file license.lgpl included in the packaging
** of this file.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Lesser General Public License for more details.
** 
****************************************************************************************/

#include "declarativedbusadaptor.h"

#include <QDBusArgument>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QMetaMethod>

#include <qqmlinfo.h>
#include <QtDebug>

#include "declarativedbusinterface.h"

/*!
  \qmltype DBusAdaptor
  \inqmlmodule DBus
  \inherits QDBusVirtualObject
  \brief Creates a DBus adaptor.

  DBusAdaptor lets you register a DBus object, emit signals and make methods available.

  Example usage:
  \qml
   import QtQuick 2.0
   import DBus 1.0

   Item {
       width: 800
       height: 600

       DBusAdaptor {
        service:"org.test"
        path: "/org/test"
        iface:"org.test"
       }
   }
   \endqml
*/
DeclarativeDBusAdaptor::DeclarativeDBusAdaptor(QObject *parent)
    : QDBusVirtualObject(parent), m_bus(DeclarativeDBusAdaptor::SessionBus)
{
}

DeclarativeDBusAdaptor::~DeclarativeDBusAdaptor()
{
}

/*!
  \qmlproperty string DBusAdaptor::service
  \brief Name of the dbus service.
*/
QString DeclarativeDBusAdaptor::service() const
{
    return m_service;
}

void DeclarativeDBusAdaptor::setService(const QString &service)
{
    if (m_service != service) {
        m_service = service;
        emit serviceChanged();
    }
}

/*!
  \qmlproperty string DBusAdaptor::path
  \brief Path of the dbus object to be registered.
*/
QString DeclarativeDBusAdaptor::path() const
{
    return m_path;
}

void DeclarativeDBusAdaptor::setPath(const QString &path)
{
    if (m_path != path) {
        m_path = path;
        emit pathChanged();
    }
}

/*!
  \qmlproperty string DBusAdaptor::interface
  \brief Name of the interface.
*/
QString DeclarativeDBusAdaptor::interface() const
{
    return m_interface;
}

void DeclarativeDBusAdaptor::setInterface(const QString &interface)
{
    if (m_interface != interface) {
        m_interface = interface;
        emit interfaceChanged();
    }
}

/*
    The XML service description.  This could be derived from the meta-object but since its unlikely
    to be needed most of the time this and type conversion is non-trivial it's not, and there is
    this property instead if it is needed.
*/

QString DeclarativeDBusAdaptor::xml() const
{
    return m_xml;
}

void DeclarativeDBusAdaptor::setXml(const QString &xml)
{
    if (m_xml != xml) {
        m_xml = xml;
        emit xmlChanged();
    }
}

DeclarativeDBusAdaptor::BusType DeclarativeDBusAdaptor::bus() const
{
    return m_bus;
}

void DeclarativeDBusAdaptor::setBus(DeclarativeDBusAdaptor::BusType bus)
{
    if (m_bus != bus) {
        m_bus = bus;
        emit busChanged();
    }
}

void DeclarativeDBusAdaptor::classBegin()
{
}

void DeclarativeDBusAdaptor::componentComplete()
{
    QDBusConnection conn = (m_bus == DeclarativeDBusAdaptor::SystemBus) ?
                QDBusConnection::systemBus() :
                QDBusConnection::sessionBus();

    // Register service name only if it has been set.
    if (!m_service.isEmpty()) {
        if (!conn.registerService(m_service)) {
            qmlInfo(this) << "Failed to register service" << m_service;
            qmlInfo(this) << conn.lastError().message();
        }
    }

    // It is still valid to publish an object on the bus without first registering a service name,
    // a remote process would have to connect directly to the DBus address.
    if (!conn.registerVirtualObject(m_path, this)) {
        qmlInfo(this) << "Failed to register object" << m_path;
        qmlInfo(this) << conn.lastError().message();
    }
}

QString DeclarativeDBusAdaptor::introspect(const QString &) const
{
    return m_xml;
}

QDBusArgument &operator << (QDBusArgument &argument, const QVariant &value)
{
    switch (value.type()) {
    case QVariant::String:      return argument << value.toString();
    case QVariant::StringList:  return argument << value.toStringList();
    case QVariant::Int:         return argument << value.toInt();
    case QVariant::Bool:        return argument << value.toBool();
    case QVariant::Double:      return argument << value.toDouble();
    default:
        if (value.userType() == qMetaTypeId<float>()) {
            return argument << value.value<float>();
        }
        return argument;
    }
}

bool DeclarativeDBusAdaptor::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    QVariant variants[10];
    QGenericArgument arguments[10];

    const QMetaObject * const meta = metaObject();
    const QVariantList dbusArguments = message.arguments();

    QString member = message.member();
    QString interface = message.interface();

    // Don't try to handle introspect call. It will be handled
    // internally for QDBusVirtualObject derived classes.
    if (interface == QLatin1String("org.freedesktop.DBus.Introspectable")) {
        return false;
    } else if (interface == QLatin1String("org.freedesktop.DBus.Properties")) {
        if (member == QLatin1String("Get")) {
            interface = dbusArguments.value(0).toString();
            member = dbusArguments.value(1).toString();

            const QMetaObject * const meta = metaObject();
            if (!member.isEmpty() && member.at(0).isUpper())
                member = "rc" + member;

            for (int propertyIndex = meta->propertyOffset();
                        propertyIndex < meta->propertyCount();
                        ++propertyIndex) {
                QMetaProperty property = meta->property(propertyIndex);

                if (QLatin1String(property.name()) != member)
                    continue;

                QVariant value = property.read(this);
                if (value.type() == QVariant::List) {
                    QVariantList variantList = value.toList();
                    if (variantList.count() > 0) {

                        QDBusArgument list;
                        list.beginArray(variantList.first().userType());
                        foreach (const QVariant &listValue, variantList) {
                            list << listValue;
                        }
                        list.endArray();
                        value = QVariant::fromValue(list);
                    }
                }

                QDBusMessage reply = message.createReply(QVariantList() << value);
                connection.call(reply, QDBus::NoBlock);

                return true;
            }
        } else if (member == QLatin1String("GetAll")) {
            interface = dbusArguments.value(0).toString();

            QDBusArgument map;
            map.beginMap();

            for (int propertyIndex = meta->propertyOffset();
                        propertyIndex < meta->propertyCount();
                        ++propertyIndex) {
                QMetaProperty property = meta->property(propertyIndex);

                QString propertyName = QLatin1String(property.name());
                if (propertyName.startsWith(QLatin1String("rc")))
                    propertyName = propertyName.mid(2);

                const QVariant value = property.read(this);

                map.beginMapEntry();
                map << propertyName;
                map << value;
                map.endMapEntry();
            }
            map.endMap();

            QDBusMessage reply = message.createReply(QVariantList());
            connection.call(reply, QDBus::NoBlock);

            return true;
        } else if (member == QLatin1String("Set")) {
            interface = dbusArguments.value(0).toString();
            member = dbusArguments.value(1).toString();
            QVariant value = dbusArguments.value(3);

            const QMetaObject * const meta = metaObject();
            if (!member.isEmpty() && member.at(0).isUpper())
                member = "rc" + member;

            for (int propertyIndex = meta->propertyOffset();
                        propertyIndex < meta->propertyCount();
                        ++propertyIndex) {
                QMetaProperty property = meta->property(propertyIndex);

                if (QLatin1String(property.name()) != member)
                    continue;

                if (value.userType() == qMetaTypeId<QDBusArgument>())
                    value = DeclarativeDBusInterface::parse(value.value<QDBusArgument>());

                return property.write(this, value);
            }
        }
        return false;
    }

    if (!member.isEmpty())
        member = "method_" + member;
    for (int methodIndex = meta->methodOffset(); methodIndex < meta->methodCount(); ++methodIndex) {
        const QMetaMethod method = meta->method(methodIndex);
        const QList<QByteArray> parameterTypes = method.parameterTypes();

        if (parameterTypes.count() != dbusArguments.count())
            continue;

        QByteArray sig(method.methodSignature());

        if (!sig.startsWith(member.toLatin1() + "("))
            continue;

        int argumentCount = 0;
        for (; argumentCount < 10 && argumentCount < dbusArguments.count(); ++argumentCount) {
            variants[argumentCount] = dbusArguments.at(argumentCount);
            QVariant &argument = variants[argumentCount];

            // If the variant type is a QBusArgument attempt to parse its contents.
            if (argument.userType() == qMetaTypeId<QDBusArgument>()) {
                argument = DeclarativeDBusInterface::parse(argument.value<QDBusArgument>());
            }

            const QByteArray parameterType = parameterTypes.at(argumentCount);
            if (parameterType == "QVariant") {
                arguments[argumentCount] = QGenericArgument("QVariant", &argument);
            } else if (parameterType == argument.typeName()) {
                arguments[argumentCount] = QGenericArgument(argument.typeName(), argument.data());
            } else if (parameterType == "QString" && argument.userType() == qMetaTypeId<QDBusObjectPath>()) {
                // QDBusObjectPath is not exported to QML, use QString instead.
                arguments[argumentCount] = Q_ARG(QString, argument.value<QDBusObjectPath>().path());
            } else if (parameterType == "QString" && argument.userType() == qMetaTypeId<QDBusSignature>()) {
                // QDBusSignature is not exported to QML, use QString instead.
                arguments[argumentCount] = Q_ARG(QString, argument.value<QDBusSignature>().signature());
            } else {
                // Type mismatch, there may be another overload.
                break;
            }
        }

        if (argumentCount == dbusArguments.length()) {
            if (method.methodType() == QMetaMethod::Signal) {
                return method.invoke(
                            this,
                            arguments[0],
                            arguments[1],
                            arguments[2],
                            arguments[3],
                            arguments[4],
                            arguments[5],
                            arguments[6],
                            arguments[7],
                            arguments[8],
                            arguments[9]);
            } else {
                QVariant retVal;
                bool success = method.invoke(
                            this,
                            Q_RETURN_ARG(QVariant, retVal),
                            arguments[0],
                            arguments[1],
                            arguments[2],
                            arguments[3],
                            arguments[4],
                            arguments[5],
                            arguments[6],
                            arguments[7],
                            arguments[8],
                            arguments[9]);
                if (success) {
                    QDBusMessage reply = retVal.isValid() ? message.createReply(retVal) : message.createReply();
                    QDBusConnection conn = (m_bus == DeclarativeDBusAdaptor::SystemBus) ?
                                QDBusConnection::systemBus() :
                                QDBusConnection::sessionBus();
                    conn.send(reply);
                }
                return success;
            }
        }
    }
    QByteArray signature = message.member().toLatin1();
    for (int i = 0; i < dbusArguments.count(); ++i) {
        if (i > 0)
            signature += ",";
        signature += dbusArguments.at(i).typeName();
    }

    qmlInfo(this) << "No method with the signature: method_" + signature;
    return false;
}

void DeclarativeDBusAdaptor::emitSignal(const QString &name, const QJSValue &arguments)
{
    QDBusMessage signal = QDBusMessage::createSignal(m_path, m_interface, name);
    QDBusConnection conn = (m_bus == DeclarativeDBusAdaptor::SystemBus) ?
                QDBusConnection::systemBus() :
                QDBusConnection::sessionBus();

    if (!arguments.isUndefined()) {
        signal.setArguments(DeclarativeDBusInterface::argumentsFromScriptValue(arguments));
    }

    if (!conn.send(signal))
        qmlInfo(this) << conn.lastError();
}
