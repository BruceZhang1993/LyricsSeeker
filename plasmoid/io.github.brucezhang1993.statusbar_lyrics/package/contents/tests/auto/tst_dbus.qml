/****************************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jollamobile.com>
** All rights reserved.
** 
** This file is part of Sailfish Silica UI component package.
**
** You may use this file under the terms of BSD license as follows:
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Jolla Ltd nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************************/

import QtTest 1.0
import QtQuick 2.3
import DBus 1.0



Item {
    width: 500; height: 500


    DBusAdaptor {
        id: dbusAdaptor

        service: "org.nemomobile.dbus.test"
        iface: "org.nemomobile.dbus.test.Interface"
        path: "/org/nemomobile/dbus/test"
        xml:'<interface name="org.nemomobile.dbus.test.Interfaced">
                <method name="TestBool"><arg name="result" type="b" direction="out"/></method>
                <method name="TestBoolIn"><arg name="argument" type="b" direction="in"/></method>
                <method name="TestString"><arg name="result" type="s" direction="out"/></method>
                <signal name="Hello"> <arg name="result" type="s" direction="out"/></signal>
             </interface>'
        bus: DBusAdaptor.SessionBus
        function method_TestBoolIn(argument){
            console.log("shiiittt: " + argument)
        }

        function method_TestBool(){
            return true
        }
        function method_TestString(){
            return "asdf"
        }

        function emitHello(){
            emitSignal("Hello", "foo")
        }
    }

    DBusInterface {
        id: dbusInterface
        service: "org.nemomobile.dbus.test"
        iface: "org.nemomobile.dbus.test.Interface"
        path: "/org/nemomobile/dbus/test"
        bus: DBusInterface.SessionBus
        property string helloResult: "bar"

        function signal_Hello(result){
            dbusInterface.helloResult = result
        }
    }

    TestCase{
        name:"Test DbusConnection"

        function test_boolMethodIn(){
            compare(dbusInterface.syncTypedCall("TestBoolIn", {type:"b", value:true})[0], false)
        }

        function test_boolMethod(){
            compare(dbusInterface.syncTypedCall("TestBool", undefined)[0] , true)
        }
        function test_stringMethod(){
            console.log("test string" + dbusInterface.syncTypedCall("TestString", undefined)[0])
            compare(dbusInterface.syncTypedCall("TestString", undefined)[0] , "asdf")
        }

        function test_emitString(){
            dbusAdaptor.emitHello()
            tryCompare(dbusInterface, "helloResult", "foo", 300, "Unable to Receive String Signal")
        }
    }
}
