import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtMultimedia 5.0

import QRCodeDecoder 1.0
import CustomFilters 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480

    Camera {
        id: camera
        captureMode: Camera.CaptureVideo
    }

    VideoOutput {
        source: camera
        focus : visible // ensure the VideoOutput gets keyboard focus
        anchors.fill: parent
        filters: [ videoFilter ]
    }

    Item {
        width: parent.width
        height: 100
        anchors.bottom: parent.bottom
        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "Start"
                onClicked: camera.start()
            }
            Button {
                text: "Stop"
                onClicked: camera.stop()
            }
        }
    }

    VideoFilter {
        id: videoFilter
        onNewImageArrived: console.log(decoder.decodeImageQML(image))
    }

    QRCodeDecoder {
        id: decoder
    }
}
