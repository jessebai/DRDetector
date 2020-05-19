import QtQuick 2.0
import QtQuick.Controls 2.4

Rectangle{
    color:"transparent"
    width:400
    height:400
    property int upper_bound
    property int lower_bound
    property string name
    property string unit
    property string imagesource
    Dial{
        id: dial
        width:parent.width
        height:width
        anchors.horizontalCenter: parent.horizontalCenter
        background: Rectangle {
                x: dial.width / 2 - width / 2
                y: dial.height / 2 - height / 2
                width:parent.width
                height:width
                color: "transparent"

                opacity: dial.enabled ? 1 : 0.3

                Image{
                    height:parent.height
                    width:parent.height
                    anchors.centerIn: parent
                    source:imagesource
                }
            }

            handle: Rectangle {
                id: handleItem
                x: dial.background.x + dial.background.width / 2 - width / 2
                y: dial.background.y + dial.background.height / 2 - height / 2
                width: 40
                height: 200
                color: "transparent"
                antialiasing: true
                opacity: dial.enabled ? 1 : 0.3
                transform: [
                    Translate {
                        y: -Math.min(dial.background.width, dial.background.height) * 0.4 + handleItem.height / 2
                    },
                    Rotation {
                        angle: dial.angle
                        origin.x: handleItem.width / 2
                        origin.y: handleItem.height / 2
                    }
                ]
                Image{
                    height:parent.height
                    width:parent.width
                    source:"qrc:/images/pointer.png"
                }
            }
    }
    Rectangle{
        color:"transparent"
        height:50
        width:200

        anchors.horizontalCenter: parent.horizontalCenter
        y:300
        Row{
            spacing:10
            Text{
                anchors.verticalCenter: parent.verticalCenter
                text: name
                color:"white"
            }

            TextField{
                id: value
                width:100
                height:40
            }
            Text{
                anchors.verticalCenter: parent.verticalCenter
                color:"white"
                text: unit
            }
        }
    }

}
