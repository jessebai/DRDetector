import QtQuick 2.0
import QtQuick.Controls 2.4

Button{
    property string imagesource
    implicitWidth: parent.width
    implicitHeight: implicitWidth
    contentItem: Rectangle{
        color:"transparent"
        Image{
            height:parent.height*0.5
            width:parent.height*0.5
            anchors.centerIn: parent
            source:imagesource
        }
    }

    background: Rectangle {
        color: down?"darkslateblue":"midnightblue"
        radius: 2
    }
}
