import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import DcmImageView 1.0
import device.XRayDetector 1.0


import "modules"
import "custom"
import data.RealtimeImage 1.0



ApplicationWindow {
    visible: true
    visibility: "Maximized"
    title: "药剂灌装线自动化DR检验系统"

    QtObject{
        id:theme

        property string font_family: "微软雅黑"
    }

    StackLayout{
        height:parent.height
        width:parent.width-nav_bar.width
        anchors.left:nav_bar.right
        id:all_views
        currentIndex: 0
        ImagePane{
            id:image_view
        }

        DetectorPane{
            id:detector_view
        }

        Connections{
            target: detector_view
        }
    }
    Rectangle{
        id:nav_bar
        color: "midnightblue"
        width: 100
        height:parent.height
        anchors.left: parent.left
        Column{
            anchors.centerIn: parent
            width: parent.width

            spacing:5
            Repeater{
                model:[
                    {imagesource:"qrc:/images/portrait.png"},
                    {imagesource:"qrc:/images/detector.png"}
                ]
                delegate: NavButton{
                    imagesource:modelData.imagesource
                    down: index==all_views.currentIndex
                    onClicked:{
                        all_views.currentIndex = index
                    }
                }
            }
        }
    }
}

