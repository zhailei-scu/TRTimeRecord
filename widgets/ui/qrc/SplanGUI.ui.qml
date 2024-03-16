import QtQuick 2.8
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import Qt5Compat.GraphicalEffects
import QtTest 1.0
Window {
    id: splanGUI
    visible:true
    width: 480
    height: 272
    color: "red"
    flags:Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    Rectangle {
        id: background
        color: "#333333"
        anchors.fill: parent
    }

    TestEvent {
       id: test
    }

    Image {
        id: backgroundcutout
        y: -2
        source: "qrc:/img/backgroundcutout.png"
        anchors.horizontalCenterOffset: 2
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Image {
        id: washdrumoutlne
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenterOffset: -30
        source: "qrc:/img/washdrumoutlne.png"

        Text {
            id: text1
            x: 53
            y: 76
            visible: true
            color: "#e544a1"
            text: qsTr("WelCome!")
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: washdrumoutlne.verticalCenter
            anchors.horizontalCenter: washdrumoutlne.horizontalCenter
            font.family: "Maven Pro"
        }
    }

    Text {
        id: softwareName
        y: 200
        color: "#62e34e"
        text: "Therapy time record software"
        font.pixelSize: 18
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Maven Pro"
    }

    Text {
        id: license
        y: 238
        color: "#a29f9f"
        text: "Hefei ion medical center @License:MIT"
        font.pixelSize: 12
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Arial"
    }

    Text {
        id: authors1
        x: 392
        y: 218
        color: "#888383"
        text: "@Lei Zhai"
        font.pixelSize: 10
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "Arial"
    }

    Text {
        id: authors2
        x: 392
        y: 238
        color: "#888383"
        text: "@Chao Shan"
        font.pixelSize: 10
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "Arial"
    }

    Text {
        id: authors3
        x: 392
        y: 258
        color: "#888383"
        text: "@Yangfan Zhang"
        font.pixelSize: 10
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "Arial"
    }

    Timedate {
        id: datetimeinstance_running
        x: 425
        y: 8
        width: 47
        height: 30
    }

    LinearGradient {
        anchors.fill: background
        start: Qt.point(0, parent.height/2)
        end: Qt.point(parent.width, parent.height/2)
        gradient: Gradient {
                //position可以更改线性透明的位置， color 就是字体的颜色(两边应该是透明的)
                //这里是字体后面背景
                GradientStop { position: 0.0; color: Qt.rgba(255,255,255,0.1) }
                GradientStop { position: 0.3; color: "blue" }
                GradientStop { position: 0.7; color: "blue" }
                GradientStop { position: 1.0; color: Qt.rgba(255,255,255,0.1) }
        }
    }

    PathAnimation{
            id: pathTextMove
            target: linearLabel
            //动画的持续时间
            duration: 6000
            //动画持续的次数
            loops: 2000
            orientationEntryDuration: 0;
            orientationExitDuration:  0;
            orientation: PathAnimation.LeftFirst
            path:Path{
                startX: splanGUI.width + linearLabel.width/2
                startY: splanGUI.height/2 - linearLabel.height/2
                PathLine{
                    x: 0 - linearLabel.width
                    y: splanGUI.height/2 - linearLabel.height/2
                }
            }
            running: true
    }

    Component.onCompleted:{
        test.mouseClick(splanGUI, 0, 0, Qt.NoButton, Qt.NoModifier, 2000);
        splanGUI.close();
    }
}
