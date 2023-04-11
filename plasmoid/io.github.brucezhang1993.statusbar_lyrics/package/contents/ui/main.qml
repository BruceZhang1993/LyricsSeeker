import QtQuick 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import DBus 1.0

PlasmaComponents.Label {
    id: label1
    text: "Hello World!"

    DBusInterface {
        id: dbusInterface
        service: "io.github.brucezhang1993.LyricsSeeker"
        iface: "io.github.brucezhang1993.LyricsSeeker"
        path: "/"
        bus: DBusInterface.SessionBus
    }

    Timer {
        interval: 500; running: true; repeat: true
        onTriggered: label1.text = dbusInterface.syncTypedCall("lyricsCurrentLine", {type: "s", value: ""})[0]
    }
}