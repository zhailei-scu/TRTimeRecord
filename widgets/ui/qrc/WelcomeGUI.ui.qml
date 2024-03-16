import QtQuick 2.8
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import Qt5Compat.GraphicalEffects

QtObject{
    id:root

    property QtObject $splanGUIScreen: SplanGUI{}


    Component.onCompleted:{
        console.log("45678");
    }
}


