import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import DcmImageView 1.0
import device.XRayDetector 1.0
import device.XRayGenerator 1.0
import device.PLCController 1.0
import Scheduler 1.0


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

        GeneratorPane{
            id:generator_view
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
                    {imagesource:"qrc:/images/detector.png"},
                    {imagesource:"qrc:/images/generator.png"}
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
        Rectangle{
            id: schedule_pane
            anchors.bottom: nav_bar.bottom
            anchors.left: nav_bar.left
            anchors.right: nav_bar.right
            height: nav_bar.height/4
            color:"#003170"
            Scheduler{
                id: scheduler

                onSendFilterMission: {
                    PLCController.setFilterMission(count, bottleFlags)
                }
            }
            Connections{
                target: RealtimeImage
                onImgProcComplete:{
                    XRayGenerator.disableXRay();
                    console.log("move on");
                    scheduler.pushResult(RealtimeImage.getTargetCount(), RealtimeImage.getBottleFlags());
                    PLCController.setNextCycleCounter(RealtimeImage.getTargetCount());
                }
            }
            Connections{
                target: PLCController
                onTargetReady:{
                    XRayGenerator.setVoltage(600)
                    XRayGenerator.setCurrent(3000)
                    XRayGenerator.enableXRay()
                }
                onFilterReady:{
                    scheduler.setFilterReady()
                }
            }
           Connections{
               target: XRayGenerator
               onFullPower:{
                   console.log("full power")
                   XRayDetector.startAquisition()
               }
           }

            Column{
                anchors.centerIn: parent
                width:parent.width-10
                spacing: 20
                ImageButton{
                    imagesource:"qrc:/images/start.png"
                    onClicked:{
                        PLCController.startPipeline()
                    }
                }
                ImageButton{
                    imagesource:"qrc:/images/stop.png"
                    onClicked:{
                        PLCController.stopPipeline()
                    }
                }
            }
        }
    }
}

