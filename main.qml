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

    property int detectedTags: 0
    property string lastTag: ""

    Rectangle
    {
        id: bgRect
        color: "white"
        anchors.fill: videoOutput
    }

    Text
    {
        id: text1
        wrapMode: Text.Wrap
        font.pixelSize: 20
        anchors.top: parent.top
        anchors.left: parent.left
        z: 50
        text: "Tags detected: " + detectedTags
    }
    Text
    {
        id: fps
        font.pixelSize: 20
        anchors.top: parent.top
        anchors.right: parent.right
        z: 50
        text: (1000 / zxingFilter.timePerFrameDecode).toFixed(0) + "fps"
    }

    Camera
    {
        id:camera
        focus {
            focusMode: CameraFocus.FocusContinuous
            focusPointMode: CameraFocus.FocusPointAuto
        }
    }

    VideoOutput
    {
        id: videoOutput
        source: camera
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
            color: "green"
            opacity: 0.2
            width: 240
            height: 240
            anchors.centerIn: parent
            radius: 16
        }
    }

    QZXingFilter
    {
        id: zxingFilter
        captureRect: {
            videoOutput.contentRect;
            videoOutput.sourceRect;
            return videoOutput.mapRectToSource(videoOutput.mapNormalizedRectToItem(Qt.rect(
                                                                                       0.25, 0.25, 0.5, 0.5
                                                                                       )));
        }

        decoder {
            enabledDecoders: QZXing.DecoderFormat_EAN_13 | QZXing.DecoderFormat_CODE_39 | QZXing.DecoderFormat_QR_CODE

            onTagFound: {
                console.log(tag + " | " + decoder.foundedFormat() + " | " + decoder.charSet());

                window.detectedTags++;
                window.lastTag = tag;
            }

            tryHarder: false
        }

        onDecodingStarted:
        {
            // console.log("started");
        }

        property int framesDecoded: 0
        property real timePerFrameDecode: 0

        onDecodingFinished:
        {
            timePerFrameDecode = (decodeTime + framesDecoded * timePerFrameDecode) / (framesDecoded + 1);
            framesDecoded++;
            //console.log("frame finished: " + succeeded, decodeTime, timePerFrameDecode, framesDecoded);
        }
    }

    Text
    {
        id: text2
        wrapMode: Text.Wrap
        font.pixelSize: 20
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        z: 50
        text: "Last tag: " + lastTag
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
        font.pixelSize: Screen.pixelDensity * 5
    }

    Rectangle {
        id: connectButton
        width: parent.width * 0.8
        height: 55
        radius: 10
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 55
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                modalBox.state = "open"
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

        AnimatedImage {
            id: loader
            height: 20
            width: 120
            anchors.bottom: parent.bottom

            source: "qrc:/resources/loader.gif"
        }

        ListView {
            id: listView
            anchors {
                fill: parent
                margins: 20
                bottomMargin: 40
            }
            clip: true

//            model: ListModel {
//                ListElement {name: "HC06"; address: "02:51:A1:C5:68:C9"; rssi: -56; isValid: true}
//                ListElement {name: "HC06"; address: "02:51:A1:C5:68:C9"; rssi: -56; isValid: true}
//                ListElement {name: "HC06"; address: "02:51:A1:C5:68:C9"; rssi: -56; isValid: true}
//                ListElement {name: "HC06"; address: "02:51:A1:C5:68:C9"; rssi: -56; isValid: true}
//                ListElement {name: "HC06"; address: "02:51:A1:C5:68:C9"; rssi: -56; isValid: true}
//            }
            model: BluetoothList {}

            spacing: 3
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
                        topMargin: 5
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
                        bottomMargin: 5
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
            }
            footer: Item {

            }
        }
    }
}
