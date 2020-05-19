import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import device.XRayDetector 1.0
import "../custom"
import data.RealtimeImage 1.0

Pane{
    id: detector_pane
    property bool detectorActive: false
    background: Rectangle{
        color: "white"
    }
    Connections{
        target:XRayDetector
        onDeviceConnected: {
            device_status.text = "已连接"
            detectorActive = true
        }
        onConnectFailed:{
            device_status.text = error
        }

        onModeUpdated: {
            mode_list.clear()
            for(var i=0; i<XRayDetector.modeCount; i++){
                mode_list.append({modeDesc:XRayDetector.getModeDesc(i)});
            }
            if(XRayDetector.activeModeIdx>=0){
                mode_combo.currentIndex = XRayDetector.activeModeIdx;
            }else{
                mode_combo.currentIndex = 0
            }
            mode_pane.loadModeInfo(mode_combo.currentIndex);
        }

        onCalibProgressing: {
            calib_progress.to = nTotalFrmNum
            calib_progress.value = nCurFrmNum
        }

        onCalibrationFinish: {
            feedback.text = "校准完成"
            calib_progress.value = 0
            calib_progress.visible = false
        }

        onNewFrame: {
            RealtimeImage.loadRealtimeImage(frmAddr, nWidth, nHeight)
        }
    }

    Rectangle{
        id: comboBox_rec
        anchors.top: detector_pane.top
        anchors.left: detector_pane.left
        height: detector_pane.height/20
        width: detector_pane.width

        ComboBox{
            id: camera_select
            width: 300
            currentIndex: 0

            model: ["X射线探测器", "海康威视相机"]

            onCurrentIndexChanged: {
                if(camera_select.currentIndex === 0){
                    function_pane.visible = true
                    mode_pane.visible = true
                    mvsCamera_pane.visible = false
                } else {
                    function_pane.visible = false
                    mode_pane.visible = false
                    mvsCamera_pane.visible = true
                }
            }
        }
    }

    Rectangle{
        anchors.top: comboBox_rec.bottom
        anchors.bottom: detector_pane.bottom
        width: detector_pane.width
        Row{
            anchors.fill:parent
            Rectangle{
                id: function_pane
                width: parent.width/3
                color: "white"
                height:parent.height
                visible: true
                ColumnLayout{
                    Button{
                        id: init_device_button
                        text:"连接"
                        onClicked: {
                            enabled: false
                            XRayDetector.initDevice();
                        }
                    }
                    Label{
                        id:device_status
                        text:"未连接"
                    }

                    Button{
                        text: "暗场校准"
                        onClicked: {
                            feedback.text = "正在校准"
                            calib_progress.visible = true
                            XRayDetector.darkCalibration()
                        }
                    }
                    Button{
                        text: "增益校准"
                        onClicked: {
                            feedback.text = "正在校准"
                            calib_progress.visible = true
                            feedback.text = XRayDetector.gainCalibration()
                        }
                    }

                    Button{
                        text:"拍摄"
                        onClicked:{
                            XRayDetector.startAquisition()
                        }
                    }

                    Label{
                        id:feedback
                        text:"ready"
                    }


                    ProgressBar{
                        id:calib_progress
                        visible: false
                        background: Rectangle{
                            implicitWidth: 200
                            implicitHeight: 5
                            color:"#e6e6e6"
                            radius:3
                        }
                        contentItem: Item{
                            implicitWidth: 200
                            implicitHeight: 5
                            Rectangle{
                                width: calib_progress.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                color: "#17a81a"
                            }
                        }
                    }
                }
            }
            Rectangle{
                id: mode_pane
                color:"white"
                width:parent.width/3
                height:parent.height
                x:width
                visible: true
                Column{
                    spacing: 50
                    ComboBox{
                        id: mode_combo
                        enabled: detectorActive
                        width: 300
                        currentIndex: 0

                        model: ListModel{
                            id: mode_list
                        }
                        onCurrentIndexChanged: {
                            mode_pane.loadModeInfo(currentIndex)
                        }
                    }
                    Row{
                        Text{
                            text:"设置帧率"
                        }
                        TextField{
                            id: framerate
                            text: "1.0"
                            selectByMouse: true

                            onFocusChanged: {
                                if(focus){
                                    selectAll()
                                }
                            }

                            onTextChanged:{
                                if(!acceptableInput){
                                    text = "1.0"
                                }
                            }

                        }
                    }
                    Row{
                        Text{
                            text:"设置触发模式"
                        }
                        ComboBox{
                            id:trigCombo
                            enabled: detectorActive
                            currentIndex: 0
                            model: ListModel{
                                id: trigTypes
                            }
                        }
                    }
                    Row{
                        Text{
                            text:"设置增益等级"
                        }
                        ComboBox{
                            id:gainCombo
                            enabled: detectorActive
                            currentIndex: 0
                            model: ListModel{
                                id: gainLevels
                            }
                        }
                    }
                    Button{
                        text: "应用模式"
                        onClicked: {
                            var nRet = XRayDetector.setModeWithParam(
                                        mode_combo.currentIndex,
                                        framerate.text,
                                        trigCombo.currentText,
                                        gainCombo.currentText)
                            console.log(nRet)
                            if(nRet < 300000){
                                current_mode_info.text = XRayDetector.getActiveModeInfo()
                            }else{
                                current_mode_info.text = XRayDetector.getErrMsg()
                            }
                        }
                    }
                    Row{
                        Text{
                            text:"当前模式"
                        }
                        Label{
                            id:current_mode_info
                        }
                    }
                }

                function loadModeInfo(idx){
                    // frValidate.top = XRayDetector.getMaxFrmRate(idx)
                    //触发模式选项
                    var trigs = XRayDetector.getTrigTypes(idx)
                    trigTypes.clear()
                    var defaultTrig = XRayDetector.getDefaultTrigType(idx)
                    for(var t in trigs){
                        trigTypes.append({trigVal:trigs[t]})
                        if(trigs[t]===defaultTrig){
                            trigCombo.currentIndex = t
                        }
                    }
                    //增益等级选项
                    gainLevels.clear()
                    var gains = XRayDetector.getGainlevels(idx)
                    var defaultGain = XRayDetector.getDefaultGainLevel(idx)
                    for(var g in gains){
                        gainLevels.append({gainVal:gains[g]})
                        if(gains[g]===defaultGain){
                            gainCombo.currentIndex = g;
                        }
                    }
                    framerate.text = XRayDetector.getMaxFrmRate(idx)
                }

            }
            Rectangle{
                id: mvsCamera_pane
                color:"white"
                height: parent.height
                width:parent.width/3
                visible: false
                Column{
                    spacing: 10
                    Button{
                        text: "连接设备"
                        onClicked: {
                            var sRet = MVSCamera.findDevice()
                            console.log(sRet);

                            var nRet = MVSCamera.openDevice()
                            if(nRet === 0){
                                console.log("设备连接成功")
                            }
                            MVSCamera.setParameterQml(10000, 0, 4.4)
                            MVSCamera.setParameter();
                        }
                    }

                    Button{
                        text: "关闭设备"
                        onClicked: {
                            var nRet = MVSCamera.closeDevice()
                            console.log(nRet)
                        }
                    }

                    Row{
                        spacing: 50
                        RadioButton{
                            id: continueCheckBox
                            text: "连续模式"
                            onClicked: {
                                var nRet = MVSCamera.continusMode()
                                console.log(nRet)
                            }
                        }

                        RadioButton{
                            id: triggerCheckBox
                            text: "触发模式"
                            onClicked: {
                                var nRet = MVSCamera.triggerMode()
                                console.log(nRet)
                            }
                        }
                    }

                    Button{
                        text: "开始采集"
                        onClicked: {
                            MVSCamera.startGrabbing()
                            console.log("startGrabbing")
                        }
                    }

                    Button{
                        text: "停止采集"
                        onClicked: {
                            MVSCamera.stopGrabbing()
                            console.log("stopGrabbing")
                        }
                    }

                    Row{
                        spacing: 50
                        CheckBox{
                            text: "软触发"
                            onClicked: {
                                var nRet = MVSCamera.softwareTrigger()
                                console.log(nRet)
                            }
                        }

                        Button{
                            text: "软触发一次"
                            onClicked: {
                                var nRet = MVSCamera.softwareOnce()
                                console.log(nRet)
                            }
                        }
                    }

                    Button{
                        text: "图片保存为BMP"
                        onClicked: {
    //                        MVSCamera.saveBmp()
                            MVSCamera.getImageBuffer()
                        }
                    }

                    Row{
                        spacing: 50
                        Button{
                            text: "参数获取"
                            onClicked: {
                                MVSCamera.getParameter()
                                var expo = MVSCamera.getExposureTimeQml()
                                console.log(expo)
                                explosureTimeInput.text = MVSCamera.getExposureTimeQml()

                                var gain = MVSCamera.getGainQml()
                                console.log(gain)
                                gainInput.text = MVSCamera.getGainQml()

                                var frameRate = MVSCamera.getFrameRateTimeQml()
                                console.log(frameRate)
                                frameRateInput.text = MVSCamera.getFrameRateTimeQml()
                            }
                        }

                        Button{
                            text:"参数设置"
                            onClicked: {
                                MVSCamera.setParameterQml(explosureTimeInput.text, gainInput.text, frameRateInput.text)
                                MVSCamera.setParameter()
                            }
                        }
                    }

                    Row{
                        Text {
                            text: "曝光"
                        }

                        TextField{
                            id: explosureTimeInput
                            text: "0"

                            selectByMouse: true

                            onFocusChanged: {
                                if(focus){
                                    selectAll()
                                }
                            }
                        }
                    }

                    Row{
                        Text {
                            text: "增益"
                        }

                        TextField{
                            id: gainInput
                            text: "0"

                            selectByMouse: true

                            onFocusChanged: {
                                if(focus){
                                    selectAll()
                                }
                            }

                        }
                    }

                    Row{
                        Text {
                            text: "帧率"
                        }

                        TextField{
                            id: frameRateInput
                            text: "0"

                            selectByMouse: true

                            onFocusChanged: {
                                if(focus){
                                    selectAll()
                                }
                            }

                        }
                    }


                }


            }
        }

    }

}
