import QtQuick 2.8
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import Qt5Compat.GraphicalEffects
import QtTest 1.0
Window {
    readonly property  real widthSet: 480;
    readonly property real heightSet: 272;
    readonly property real length: 0.5*Math.sqrt(widthSet*widthSet + heightSet*heightSet)
    property string colorSet: "red";
    property real binSizeX:0.005;
    property int ix_moveUP: 0.5/binSizeX;
    property int ix_moveDown: 0.5/binSizeX;
    property real angle: Math.atan(heightSet/widthSet);
    property point startPoint: Qt.point(0,0)
    property point endPoint: Qt.point(widthSet,heightSet)

    id: splanGUI
    visible:true
    width: widthSet
    height: heightSet
    color: "red"
    flags:Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    Rectangle {
        id: background
        //color: "#333333"
        color:colorSet;
        anchors.fill: parent
        LinearGradient{
            anchors.fill: parent
            start:startPoint;
            end:endPoint;

            gradient: Gradient {
                GradientStop {
                    position: 0;
                    color: "#e544a1";
                }

                GradientStop {
                    position: ix_moveUP*binSizeX;
                    color: "#333333";
                }

                GradientStop {
                    position: ix_moveDown*binSizeX;
                    color: "#333333";
                }

                GradientStop {
                    position: 1.0;
                    color: "#e544a1";
                }
            }
        }

        /*
        gradient: Gradient {
            id:gradientVertical

            GradientStop {
                position: 0;
                color: "#e544a1";
            }

            GradientStop {
                position: ix_moveUP*binSizeX;
                color: "#333333";
            }

            GradientStop {
                position: ix_moveDown*binSizeX;
                color: "#333333";
            }

            GradientStop {
                position: 1.0;
                color: "#e544a1";
            }
        }
        */

        /*
        PropertyAnimation{
            id:backgroundChange
            target:background
            property:"color"
            easing.type: Easing.InQuad
            to:"green"
            duration:1000
            loops:Animation.Infinite
            running: true
        }
        */

        function changeBackGround(){
            //backgroundChange.start();
            //colorSet = "blue";

            ix_moveUP -= 3;
            ix_moveDown += 3;
            angle -= 5;
        }
    }

    TestEvent {
       id: test
    }

    Item {
        id: timeContainerFlush

        Timer {
            id: timer
            interval: 100
            running: true
            repeat: true

            onTriggered: {
                background.changeBackGround();
                endPoint = Qt.point(widthSet/2 + length*Math.cos(Math.PI*angle/180),heightSet/2 - length*Math.sin(Math.PI*angle/180));
            }
        }
    }

    /*
    Image {
        id: backgroundcutout
        y: -2
        source: "qrc:/img/backgroundcutout.png"
        anchors.horizontalCenterOffset: 2
        anchors.horizontalCenter: parent.horizontalCenter
    }
    */

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
        text: "Therapy time recording"
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

    Component.onCompleted:{
        test.mouseClick(splanGUI, 0, 0, Qt.NoButton, Qt.NoModifier, 2000);
        splanGUI.close();
    }
}
