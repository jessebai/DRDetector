import QtQuick 2.0
import QtGraphicalEffects 1.0

Item{
    default property alias content: content_area.data
    Rectangle{
        id:box
        color: "#0c1c65"
        width: parent.width-50
        height:parent.height-50
        anchors.centerIn: parent
        Item{
            id: content_area
            width:parent.width-50
            height:parent.height-50
            anchors.centerIn: parent
        }
    }

    DropShadow {
            anchors.fill: box
            cached: true
            horizontalOffset: 3
            verticalOffset: 3
            radius: 8.0
            samples: 16
            color: "#80000000"
            smooth: true
            source: box
        }
}

