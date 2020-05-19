import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4

import device.XRayGenerator 1.0

import "../custom"

Pane{
    id: generator_pane
    padding:0
    background: Rectangle{
        color: "#000E59"
    }

    Row{
        Column{
            FuncContainer{
                id: watch_dog_area
                //watch dog and xray training
                width:3/4 * generator_pane.width
                height: 1/3 * generator_pane.height

                Row{
                    height:parent.height
                    Rectangle{
                        //watch dog
                        color:"transparent"
                        height:parent.height
                        width: 0.4 * watch_dog_area.width
                        Column{
                            anchors.centerIn:parent
                            spacing:20
                            Text{
                                text:"WatchDog"
                                font.pointSize: 20
                                font.family: theme.font_family
                                color:"white"
                            }
                            Switch{
                                checked:true
                            }
                        }
                    }
                    Rectangle{
                        color:"purple"
                        height: parent.height
                        width: 3
                    }
                }
            }
            FuncContainer{
                //xray operate and state monitor
                width: 3/4 * generator_pane.width
                height: 2/3 * generator_pane.height

                Column{
                    width:100
                    spacing:50
                    anchors.verticalCenter: parent.verticalCenter
                    ImageButton{
                        width:parent.width
                        height: width
                        imagesource:"qrc:/images/xray_enable.png"
                    }
                    ImageButton{
                        width:parent.width
                        height:width
                        imagesource: "qrc:/images/xray_disable.png"
                    }
                    ImageButton{
                        width:parent.width
                        height:width
                        imagesource: "qrc:/images/xray_reset.png"
                    }
                }
                Rectangle{
                    color:"purple"
                    height:parent.height
                    width:3
                    x: 100
                }

                Rectangle{
                    id: dial_region
                    color: "transparent"
                    height:parent.height - 120
                    width: parent.width-120
                    x: 120
                    Row{
                        anchors.centerIn: parent
                        spacing:50
                        GeneratorDial{
                            id:voltage
                            name: "电压："
                            unit: "kV";
                            imagesource:"qrc:/images/voltage_dial.png";

                        }
                        GeneratorDial{
                            id:current
                            name: "电流："
                            unit: "mA"
                            imagesource:"qrc:/images/current_dial.png"

                        }
                    }
                }

                Rectangle{
                    color:"purple"
                    x:120
                    y: parent.height-100
                    height:3
                    width: parent.width-120
                }

                Rectangle{
                    color:"transparent"
                    x:120
                    y:parent.height-100
                    height:100
                    width:parent.width-120
                    Row{
                        anchors.centerIn: parent
                        Image{
                            height:90
                            width:90
                            source:"qrc:/images/temperature_ico.png"
                        }
                        Slider{
                            anchors.verticalCenter: parent.verticalCenter
                            height:50
                            width:500
                            enabled: false

                            background:Rectangle{
                                color:"transparent"
                                Image{
                                    source:"qrc:/images/thermo_bak.png"
                                }
                            }

                            handle: Rectangle{
                                x: parent.visualPosition*(parent.width)
                                y: (parent.height - height) / 2
                                implicitHeight: 15
                                implicitWidth: 10
                                color:"transparent"
                                Image{
                                    source:"qrc:/images/thermo_indicator.png"
                                }
                            }
                        }
                        Text{
                            anchors.verticalCenter: parent.verticalCenter
                            text:"20.5"
                            color:"white"
                            font.family: theme.font_family
                            font.pointSize: 20
                        }
                    }
                }

            }

        }
        FuncContainer{
            width: 1/4* generator_pane.width
            height: generator_pane.height
            Column{
                width:parent.width
                spacing:10
                Text{
                    text:"错误列表"
                    font.pointSize: 16
                    font.family: theme.font_family
                    color: "white"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Rectangle{
                    color:"purple"
                    height:3
                    width:parent.width
                }
            }
        }
    }
}
