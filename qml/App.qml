import QtQuick
import QtQuick.Controls
import Df64Tester

Window {
    width: mainScreen.width
    height: mainScreen.height

    visible: true
    title: "Df64Tester"

    id: mainWindow

    Constants {
        id: cnst
    }

    Rectangle {
        id: mainScreen
        width: cnst.width
        height: cnst.height

        color: cnst.backgroundColor

        Button {
            id: btnDetect
            x: 18
            y: 15
            width: 117
            height: 52
            text: qsTr("COM-ports")
            onClicked: {
                qmlLayer.processGetPorts()
            }
        }

        Text {
            id: txtCOMLest
            x: 172
            y: 15
            width: 561
            height: 52
            text: qmlLayer.listComPortsString
            font.pixelSize: 12
        }

        Button {
            id: btnOpen
            x: 18
            y: 85
            width: 117
            height: 52
            text: qsTr("Открыть")
            onClicked: {
                qmlLayer.openPort(cbOpen.currentValue)
            }

        }

        ComboBox {
            id: cbOpen
            x: 172
            y: 85
            width: 311
            height: 52
            model: qmlLayer.listComPorts
        }
    }

    TextArea {
        id: txtLog
        x: 18
        y: 153
        width: 764
        height: 431
        color: "black"
        //text: qsTr("Многострочный\nтекст")
        wrapMode: Text.WordWrap
        placeholderText: qsTr("Text Area")
    }

    function execGetPorts() {
        qmlLayer.processGetPorts()
    }


}

