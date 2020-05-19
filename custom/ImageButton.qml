import QtQuick 2.0
import QtQuick.Controls 2.4

Button{
    property string imagesource
    implicitHeight: 90
    implicitWidth: 90
    contentItem: Rectangle{
        color:"transparent"
        Image{
            height:parent.height
            width:parent.height
            anchors.centerIn: parent
            source:imagesource
        }
    }

    background: Rectangle {
        color: "transparent"
        radius: 2
    }
}

