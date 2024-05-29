import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtMultimedia 5.9

import QZXing 3.3
import Bluetooth 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    title: "Qt QZXing Filter Test"

    Component.onCompleted: {
        activityManager.keepScreenOn(true);
    }

    onClosing: {
        activityManager.keepScreenOn(false);
    }

    property string lastTag: ""
    property int detectedTags: 0

    ActivityManager {
        id: activityManager
    }

    Text {
        id: text1
        wrapMode: Text.Wrap
        font.pixelSize: 14
        anchors.top: parent.top
        anchors.left: parent.left
        z: 50
        text: "Tags detected: " + detectedTags
    }

    ComboBox {
        id: comboBox
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        currentIndex: -1 // Ничего не выбрано изначально
    }

    Text {
        id: fps
        font.pixelSize: 20
        anchors.top: parent.top
        anchors.right: parent.right
        z: 50
        text: (1000 / zxingFilter.timePerFrameDecode).toFixed(0) + "fps"
    }

    VideoOutput {
        id: videoOutput
        source: Camera {
            id:camera
            focus {
                focusMode: CameraFocus.FocusContinuous
                focusPointMode: CameraFocus.FocusPointAuto
            }
        }
        anchors.top: text1.bottom
        anchors.bottom: text2.top
        anchors.left: parent.left
        anchors.right: parent.right
        autoOrientation: true
        filters: [ zxingFilter ]

        MouseArea {
            anchors.fill: parent
            onClicked: {
                camera.focus.customFocusPoint = Qt.point(mouse.x / width,  mouse.y / height);
                camera.focus.focusMode = CameraFocus.FocusMacro;
                camera.focus.focusPointMode = CameraFocus.FocusPointCustom;
                modalBox.state = "close"
            }
        }

        Rectangle {
            id: captureZone
            color: "transparent"
            opacity: 0.5
            width: 240
            height: 240
            anchors.centerIn: parent
            radius: 16
            border.color: "white"
            border.width: 4
        }

        Text {
            id: debug
            text: qsTr("Received:")
            anchors.top: parent.top
            anchors.topMargin: 50
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    QZXingFilter {
        id: zxingFilter
        captureRect: {
            videoOutput.contentRect;
            videoOutput.sourceRect;
            return videoOutput.mapRectToSource(videoOutput.mapNormalizedRectToItem(Qt.rect( 0.25, 0.25, 0.5, 0.5 )));
        }

        decoder {
            enabledDecoders: QZXing.DecoderFormat_QR_CODE;

            onTagFound: {
                //console.log(tag + " | " + decoder.foundedFormat() + " | " + decoder.charSet());
                window.detectedTags++;
                if (comboBox.displayText === tag) bluetooth_device.sendData("ok");
                else bluetooth_device.sendData("error");
                window.lastTag = tag;
            }

            tryHarder: false;
        }

        property int framesDecoded: 0
        property real timePerFrameDecode: 0

        onDecodingFinished: {
            timePerFrameDecode = (decodeTime + framesDecoded * timePerFrameDecode) / (framesDecoded + 1);
            framesDecoded++;
        }
    }

    Text {
        id: text2
        wrapMode: Text.Wrap
        font.pixelSize: 20
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        text: qsTr("Data: " + window.lastTag)
    }

    Switch {
        text: "Autofocus"
        checked: camera.focus.focusMode === CameraFocus.FocusContinuous
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
        onCheckedChanged: {
            if (checked) {
                camera.focus.focusMode = CameraFocus.FocusContinuous
                camera.focus.focusPointMode = CameraFocus.FocusPointAuto
            } else {
                camera.focus.focusPointMode = CameraFocus.FocusPointCustom
                camera.focus.customFocusPoint = Qt.point(0.5,  0.5)
            }
        }
        font.family: Qt.platform.os === 'android' ? 'Droid Sans Mono' : 'Monospace'
    }

    Rectangle {
        id: connectButton
        width: parent.width * 0.8
        height: 75
        radius: 10
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 55
        }

        Rectangle {
            height: 20
            width: 20
            radius: 10
            color: bluetooth_device.status === BluetoothDevice.Connected ? "green" :
                                                                           bluetooth_device.status === BluetoothDevice.Connecting ? "yellow" : "gray"
            anchors {
                left: parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }

            SequentialAnimation on opacity {
                loops: Animation.Infinite
                running: bluetooth_device.status === 1
                NumberAnimation {
                    from: 1.0
                    to: 0.3
                    duration: 500 // Время анимации в миллисекундах
                }
                NumberAnimation {
                    from: 0.3
                    to: 1.0
                    duration: 500 // Время анимации в миллисекундах
                }
            }
        }

        Text {
            text: bluetooth_device.name
            anchors {
                top: parent.top
                left: parent.left
                topMargin: 10
                leftMargin: 60
            }

            font {
                bold: true
                pointSize: 18
            }
        }

        Text {
            text: bluetooth_device.status === BluetoothDevice.Connected ? qsTr("Connected") :
                                                                          bluetooth_device.status === BluetoothDevice.Connecting ? qsTr("Connecting ...") : qsTr("Disconnected")
            anchors {
                bottom: parent.bottom
                left: parent.left
                bottomMargin: 10
                leftMargin: 60
            }

            font {
                bold: false
                pointSize: 14
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                modalBox.state = "open"
            }
        }
    }

    BluetoothDevice {
        id: bluetooth_device
        onDataReceived: {
            debug.text = data;

            var text = data;
            var regex = /^\*([^,]+,)*([^,]+)\#$/;

            if (regex.test(text)) {
                text = text.slice(1, -1);
                var options = text.split(',');
                comboBox.model = options
            }
        }
    }

    Rectangle {
        id: modalBox
        width: parent.width
        height: parent.height * 0.75
        state: "close"
        color: "white"
        radius: 20
        z: 100
        anchors.horizontalCenter: parent.horizontalCenter

        states: [
            State {
                name: "open"
                PropertyChanges {
                    target: modalBox
                    y: parent.height - (height - 40)
                }
            },
            State {
                name: "close"
                PropertyChanges {
                    target: modalBox
                    y: window.height
                }
            }
        ]

        Behavior on y {
            NumberAnimation {
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }

        ListView {
            id: listView
            spacing: 3
            clip: true
            anchors {
                fill: parent
                margins: 20
                bottomMargin: 40
            }
            model: BluetoothList {
                onDeviceSelected: {
                    bluetooth_device.setSelectedDevice(deviceInfo)
                }
            }

            headerPositioning: ListView.OverlayFooter
            footerPositioning: ListView.OverlayFooter

            header: Rectangle {
                width: parent.width
                height: 50
                color: "#F0F0F0"

                Text {
                    id: labelText
                    text: qsTr("Bluetooth devices")// This is available in all editors.
                    font.pointSize: 16
                    font.bold: true
                    anchors.centerIn: parent
                }
                z: 2
            }

            delegate: Item {
                height: 55
                width: parent.width

                Rectangle {
                    width: parent.width
                    height: 2
                    color: "lightgray"
                    visible: index != 0
                }

                Text {
                    id: deviceName
                    text: model.name
                    font.pointSize: 14
                    font.bold: true
                    color: "black"
                    anchors {
                        top: parent.top
                        left: parent.left
                        topMargin: 8
                        leftMargin: 10
                    }
                }

                Text {
                    id: deviceAddress
                    text: model.address
                    font.pointSize: 12
                    color: "black"
                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                        bottomMargin: 8
                        leftMargin: 10
                    }
                }

                Text {
                    id: deviceRssi
                    text: model.rssi + "dBm"
                    font.pointSize: 16
                    font.bold: true
                    color: "black"
                    anchors {
                        right: parent.right
                        rightMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        listView.model.select(index)
                        modalBox.state = "close"
                    }
                }
            }

            footer: Rectangle {
                width: parent.width
                height: 44
                z: 2

                AnimatedImage {
                    id: loader
                    height: 44
                    width: 60
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "qrc:/resources/loader.gif"
                }
            }
        }
    }
}

