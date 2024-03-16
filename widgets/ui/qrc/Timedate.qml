import QtQuick 2.8

Item {
    id: timedate
    width: 47
    height: 29

    property string timeNow: Qt.formatDateTime(new Date(),"HH:mm:ss");

    Text {
        id: timelabel
        x: 0
        y: -1
        width: 43
        height: 16
        color: "#B8B8B8"
        font.pixelSize: 16
        horizontalAlignment: Text.AlignHCenter
        font.family: "Maven Pro";

        text: timeNow;
    }

    function currentTime() {
        timeNow = Qt.formatDateTime(new Date(),"HH:mm:ss");
        return timeNow;
    }

    Item {
        id: timeContainer

        Timer {
            id: timer
            interval: 1000
            running: true
            repeat: true

            onTriggered: {
                currentTime();
            }
        }
    }

    Component.onCompleted: {
        currentTime();
    }
}
