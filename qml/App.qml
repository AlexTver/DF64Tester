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

        // Rectangle {
            // id: infoBlock
            Text {
                id: headerTFInfo
                x: 18
                y: btnOpen.y + btnOpen.height + 5
                // width: 561
                height: 40
                verticalAlignment: Text.AlignVCenter
                text: qsTr("Подключенный модуль:")
                font.pixelSize: 12
            }
            Text {
                id: txtTFInfo
                x: headerTFInfo.x + headerTFInfo.width + 5
                y: btnOpen.y + btnOpen.height + 5
                // width: 561
                height: 40
                verticalAlignment: Text.AlignVCenter
                // text: qmlLayer.listComPortsString
                text: qmlLayer.deviceInfoString
                font.pixelSize: 12
            }
            Button {
                id: btnActivate
                x: 18
                y: headerTFInfo.y + headerTFInfo.height + 5
                width: 117
                height: 40
                text: qmlLayer.devNextState
                onClicked: {
                    qmlLayer.changeState()
                }
            }
            Text {
                id: headerTFData
                x: btnActivate.x + btnActivate.width + 5
                y: headerTFInfo.y + headerTFInfo.height + 5
                // width: 561
                height: 40
                verticalAlignment: Text.AlignVCenter
                text: qsTr("Текущие данные:")
                font.pixelSize: 12
            }
            Text {
                id: txtTFData
                x: headerTFData.x + headerTFData.width + 5
                y: headerTFInfo.y + headerTFInfo.height + 5
                // width: 561
                height: 40
                verticalAlignment: Text.AlignVCenter
                text: qmlLayer.dataString
                font.pixelSize: 12
            }
        // }
        // TextArea {
        //     id: txtLog
        //     x: 18
        //     y: 153
        //     width: 764
        //     height: 431
        //     color: "black"
        //     //text: qsTr("Многострочный\nтекст")
        //     wrapMode: Text.WordWrap
        //     placeholderText: qsTr("Text Area")
        // }
    }


    function execGetPorts() {
        qmlLayer.processGetPorts()
    }


}

