import QtQuick 2.8
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtTest 1.0
Window {
    id: splanGUI
    visible:true
    width: 480
    height: 272
    color: "red"
    flags:Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    TestEvent {
       id: test
    }

    function foo() {
        console.log('Executed Foo');
    }

    function bar() {
        console.log('Executed Bar');
    }

    Component.onCompleted:{
        console.log("123456");
        test.mouseClick(splanGUI, 0, 0, Qt.NoButton, Qt.NoModifier, 1000)
    }
}
