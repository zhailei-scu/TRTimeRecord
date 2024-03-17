import QtQuick 2.8
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import Qt5Compat.GraphicalEffects

Rectangle{
    id:operator_BackGround
    color: "#e544a1"
    anchors.fill: parent

    readonly property  real widthSet: operator_BackGround.width;
    readonly property real heightSet: operator_BackGround.height;
    readonly property real length: 0.5*Math.sqrt(widthSet*widthSet + heightSet*heightSet)
    readonly property real binSizeX:0.005;
    readonly property int binNum: 1/binSizeX;

    property int iStep0:0
    property int iStep1:0
    property real pos0:iStep0*binSizeX
    property real pos1:(0.5*binNum - iStep1)*binSizeX
    property real pos2:(0.5*binNum + iStep1)*binSizeX
    property real pos3:(binNum - iStep0)*binSizeX
    property real angle: Math.atan(heightSet/widthSet);
    property point startPoint: Qt.point(0,0)
    property point endPoint: Qt.point(widthSet,heightSet)

    LinearGradient{
        anchors.fill: parent
        start:startPoint;
        end:endPoint;

        gradient: Gradient {
            GradientStop {
                position: pos0;
                color: "#151a1e";
            }

            GradientStop {
                position: pos1;
                color: "#333333";
            }

            GradientStop {
                position: pos2;
                color: "#333333";
            }

            GradientStop {
                position: pos3;
                color: "#151a1e";
            }
        }
    }

    function changeBackGround(){

        if(iStep0 <= 0 && iStep1 < binNum/2){
            iStep1 += 1;

            pos0 = iStep0*binSizeX;
            pos1 = (0.5*binNum - iStep1)*binSizeX;
            pos2 = (0.5*binNum + iStep1)*binSizeX;
            pos3 = (binNum - iStep0)*binSizeX;
        }else{
            iStep1 = 0;

            pos0 = (0.5*binNum - iStep0)*binSizeX;
            pos1 = iStep1*binSizeX*binSizeX;
            pos2 = (binNum - iStep1)*binSizeX;
            pos3 = (0.5*binNum + iStep0)*binSizeX;

            if(iStep0 < binNum/2){
                iStep0 += 1;
            }else{
                iStep0 = 0;
                iStep1 = 1;
            }
        }

        angle -= 2;
    }


    Timer {
        id: timer
        interval: 100
        running: true
        repeat: true

        onTriggered: {
            changeBackGround();
            endPoint = Qt.point(widthSet/2 + length*Math.cos(Math.PI*angle/180),heightSet/2 - length*Math.sin(Math.PI*angle/180));
        }
    }
}
