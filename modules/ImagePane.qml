import QtQuick 2.0
import QtQuick.Controls 2.4
//import QtQuick.Dialogs 1.2
import Qt.labs.folderlistmodel 2.2
import QtQuick.Window 2.3
import Qt.labs.platform 1.0
import QtCharts 2.2
import QtQuick.Layouts 1.1

import "../custom"

import device.XRayDetector 1.0
import DcmImageView 1.0
import data.LocalImage 1.0
import data.RealtimeImage 1.0

Pane{
    id: image_view
    background: Rectangle{
        color:"grey"
    }
    //the slot to receive signal from detector
    function newRealTimeImage(frmAddr, nWidth, nHeight){
        dcm_image.newRealTimeImage(frmAddr, nWidth, nHeight)
    }
    FileDialog {
        id: fileDialog
        title: "请选择dcm文件"
        fileMode: FileDialog.OpenFile
        nameFilters: ["dcm files (*.dcm)", "All files (*)"]
        onAccepted: {
            console.log(fileDialog.fileUrl)
            console.log(fileDialog.folder)
            console.log(fileDialog.file)
            console.log("You chose: " + fileDialog.file)
            console.log(fileDialog.folder)
            album.openFile(fileDialog.file)

        }
        onRejected: {
            console.log("Canceled")
        }
        Component.onCompleted: visible = false
    }
    FileDialog {
        id: fileDialogSave
        title: "保存文件"
        fileMode: FileDialog.SaveFile
//        fileMode: FileDialog.SaveFile
        nameFilters: ["dcm files (*.dcm)", "BMP files (*.bmp)", "JPG files (*.jpg)", "PNG files (*.png)", "All files (*)"]
        onAccepted: {
            console.log(fileDialogSave.fileUrl)
            console.log(fileDialogSave.folder)
            console.log(fileDialogSave.file)
            dcm_image.saveCurImage(fileDialogSave.file, color_window.first.value, color_window.second.value)
        }
        onRejected: {
            console.log("Canceled")
        }
        Component.onCompleted: visible = false
    }
//    selectFolder : bool  以后修改
//    不同的FileDialog有不同的属性，现在我用的这个没有上述的属性
//    FileDialog {
//        id: fileDialogSavePos
//        title: "自动保存位置选择"
//        nameFilters: ["dcm files (*.dcm)", "All files (*)"]
//        onAccepted: {
//            console.log(fileDialogSave.fileUrl)
//            console.log(fileDialogSave.folder)
//            console.log(fileDialogSave.file)
//            filePos.text = "保存位置: " + fileDialog.folder
//        }
//        onRejected: {
//            console.log("Canceled")
//        }
//        Component.onCompleted: visible = false
//    }
    Drawer{
        id: menu_nav
        width: 300
        height: parent.height
        edge:Qt.RightEdge
        modal: false
        Column{
            id: menuCol
            x: 20
            spacing: 30
            Rectangle{
                width: menu_nav.width - menuCol.x
                height: 300
                border.color:"green"
                color:"transparent"
                border.width: 10
                Column{
                    spacing: 30
                    x: 50
                    y: 50
                    Button{
                        id: measureModeButton
                        text:"几何测量模式"
                        height:40
                        width: 120
                        highlighted: false
                        onClicked:{
                            highlighted = !highlighted
                            if(highlighted){
                                right_button_func.enabled = false
                                measureArea.enabled = true
                                selectMeasureLine.visible = true
                                selectMeasureArea.visible = false
                                calibrationButton.enabled = false
                                lengthMeasureButton.enabled = true
                                lengthMeasureButton.highlighted = true
                                areaMeasureButton.enabled = false
                            }
                            else{
                                right_button_func.enabled = true
                                measureArea.enabled = false
                                selectMeasureLine.visible = false
                                selectMeasureArea.visible = false
                                calibrationButton.enabled = false
                                lengthMeasureButton.enabled = false
                                areaMeasureButton.enabled = false
                                calibrationButton.highlighted = false
                                lengthMeasureButton.highlighted = false
                                areaMeasureButton.highlighted = false
                                measureLenResult.text = ""
                            }
                        }
                    }

                    Button{
                        id: calibrationButton
                        text:"比例校准"
                        height:20
                        width: 100
                        highlighted: false
                        enabled: false
                        onClicked:{
                            highlighted = !highlighted
                            if(highlighted){
                                measureArea.enabled = false
                                selectMeasureArea.visible = false
                                selectMeasureLine.visible = false
                                calibrationArea.enabled = true
                                lengthMeasureButton.enabled = false
                                areaMeasureButton.enabled = false
                            }
                            else{
                                measureArea.enabled = true
                                calibrationArea.enabled = false
                                lengthMeasureButton.enabled = true
                                areaMeasureButton.enabled = true
                            }
                        }
                    }

                    Button{
                        id: lengthMeasureButton
                        text:"长度测量"
                        height:20
                        width: 100
                        highlighted: false
                        enabled: false
                        onClicked:{
                            highlighted = !highlighted
                            if(highlighted){
                                measureArea.enabled = true
                                selectMeasureArea.visible = false
                                selectMeasureLine.visible = true
                                calibrationArea.enabled = false
                                calibrationButton.enabled = false
                                areaMeasureButton.enabled = false
                            }
                            else{
                                measureArea.enabled = false
                                selectMeasureLine.visible = false
                                calibrationButton.enabled = true
                                areaMeasureButton.enabled = true
                            }
                        }
                    }

                    Button{
                        id: areaMeasureButton
                        text:"面积测量"
                        height:20
                        width: 100
                        highlighted: false
                        enabled: false
                        onClicked:{
                            highlighted = !highlighted
                            if(highlighted){
                                measureArea.enabled = true
                                selectMeasureArea.visible = true
                                selectMeasureLine.visible = false
                                calibrationArea.enabled = false
                                calibrationButton.enabled = false
                                lengthMeasureButton.enabled = false
                            }
                            else{
                                measureArea.enabled = false
                                selectMeasureArea.visible = false
                                calibrationButton.enabled = true
                                lengthMeasureButton.enabled = true
                            }
                        }
                    }
                }


            }
            Button{
                id: imgInfo_button
                text:"添加水印"
                height: 40
                width: 150
                onClicked:{
                    subWindow.visible = true
                }
            }

            Button{
                id: saveInfo_button
                text:"设置自动保存"
                height: 40
                width: 150
                onClicked:{
                    subWindowSaveInfo.visible = true
                }
            }

            Button{
                id: signalToNoiseRatio_button
                text: "信噪比"
                height: 40
                width: 150
                onClicked: {
                    highlighted = !highlighted
                    if(highlighted){
                        snrArea.enabled = true
                        selectSNRArea.visible = true
                    }
                    else{
                        snrArea.enabled = false
                        selectSNRArea.visible = false
                    }

                }
            }
            Button{
                id: doubleWall_button
                text: "双线性像质计"
                height: 40
                width: 150
                onClicked: {
                    highlighted = !highlighted
                    if(highlighted){
                        dwArea.enabled = true
                        selectDWLine.visible = true
                    }
                    else{
                        dwArea.enabled = false
                        selectDWLine.visible = false
                    }
                }
            }
        }
    }

    Drawer{
        id: file_nav
        width: 300
        height: parent.height
        edge:Qt.RightEdge
        modal: false
        Column{
            spacing:10
            Button{
                text:"打开"
                height:20
                onClicked: {
                    fileDialog.open()
                }
            }
            Button{
                text:"保存"
                height:20
                onClicked: {
                    fileDialogSave.open()
                }
            }
        }
        Rectangle{
            anchors.fill:parent
            anchors.topMargin: 50
            color:"white"

            ListView {
                id:file_view
                anchors.fill:parent
                anchors.topMargin:30
                anchors.bottomMargin: 30
                focus: true
                currentIndex: -1

                FolderListModel {
                    id: folderModel
                    folder: fileDialog.folder
                    showDirs: false
                    nameFilters: ["*.dcm"]
                }

                model: folderModel
                delegate: ItemDelegate{
                    width: parent.width
                    text: model.fileName
                    highlighted: ListView.isCurrentItem
                    onClicked: {
                        file_view.currentIndex = index
                        console.log(model.fileURL)
                        album.openFile(model.fileURL)
                    }
                }
            }
        }

    }

    Canvas{
        z:5
        id: canvas_ruler
        width: parent.width
        height: parent.height
        onPaint: {
            var ctx = getContext("2d");
            //与后续的slidebar联动
            var rulerWidth = album.topMargin
            var m_dMeasureScale = dcm_image.getMeasureScale()
            var sizeX = dcm_image.imgWidth * m_dMeasureScale
            var sizeY = dcm_image.imgHeight * m_dMeasureScale
            console.log('sizeX: ', sizeX)
            console.log('sizeY: ', sizeY)
            var x = -album.contentX
            var y = -album.contentY
            var xy = album.topMargin
            var wX = album.contentWidth/sizeX
            var wY = album.contentHeight/sizeY
            if(sizeX < 5 || sizeX > 100000){
                sizeX = 60
                x = album.topMargin
                wX = 20
            }
            if(sizeY < 5 || sizeY > 100000){
                sizeY = 60
                xy = album.topMargin
                wY = 20
            }
//            var x = album.topMargin
//            var xy = album.topMargin
//            var wX = 20
//            var wY = 20
            var h = album.topMargin/2

            //需要修改的参数如下
            //size：控制坐标轴上的总长度尺寸大小
            //x：控制坐标轴起始点的位置
            //w：控制坐标坐标轴上尺寸的密度，影响外观，不影响尺寸，是相邻刻度线间隔
            //（w长度应该和图片长度相等，可能是计算出来的）
            //rulersize，xy，h，offset用来做外观的调整，后续再改，是固定值
            draw(ctx, rulerWidth, sizeX, sizeY, x, y, xy, wX, wY, h)
        }
    }

    function draw(ctx, rulerWidth, sizeX, sizeY, x, y, xy, wX, wY, h) {//rulerWidth config是风险点
        console.log("ruler draw")
        console.log('sizeX: ', sizeX)
        console.log('sizeY: ', sizeY)
        console.log('x: ', x)
        console.log('y: ', y)
        console.log('xy: ', xy)
        console.log('wX: ', wX)
        console.log('wY: ', wY)
        console.log('h: ', h)
//        var rulerWidth = 20

        // 画之前清空画布
        ctx.clearRect(0, 0, canvas_ruler.width, canvas_ruler.height)
        ctx.fillStyle ="transparent"
        ctx.fillRect(0, x, rulerWidth, canvas_ruler.height-rulerWidth);
        ctx.fillRect(x, 0, canvas_ruler.width-rulerWidth, rulerWidth);

        ctx.fillStyle ="#111"    // 设置画笔属性
        ctx.strokeStyle = '#333'
        ctx.lineWidth = 1
        ctx.font = " 20px sans-serif"
        for (var i = 0; i < sizeX; i++) {  //画水平
            ctx.beginPath()          // 开始一条路径
            // 移动到指定位置
            ctx.moveTo(x + i * wX, xy)
            // 满10刻度时刻度线长一些 并且在上方表明刻度
            if (i % 10 == 0) {
                // 计算偏移量
                var offset = (String(i / 10).length * 6 / 2)
                ctx.fillText(i / 10, x + i * wX - offset + 5, xy - h * 1.2 + 5)
                ctx.lineTo(x + i * wX, xy - h * 2)
            } else {
                // 满5刻度时的刻度线略长于1刻度的
                ctx.lineTo(x + i * wX, xy - (i % 5 === 0 ? 1 : 0.6) * h)
            }
            // 画出路径
            ctx.stroke()
        }

        for ( i = 0; i < sizeY; i++) {  //画垂直
            ctx.beginPath()          // 开始一条路径
            // 移动到指定位置
            ctx.moveTo(xy,y + i * wY)
            // 满10刻度时刻度线长一些 并且在上方表明刻度
            if (i % 10 == 0) {
                // 计算偏移量
                offset = (String(i / 10).length * 6 / 2)
                ctx.lineTo( xy - h * 2, y + i * wY)
                ctx.fillText(i / 10, xy - h * 1.2-5,y + i * wY - offset+20)
            } else {
                // 满5刻度时的刻度线略长于1刻度的
                ctx.lineTo(xy - (i % 5 === 0 ? 1 : 0.6) * h,y + i * wY)
            }
            // 画出路径
            ctx.stroke()
        }
        ctx.fillStyle ="gray"
        ctx.fillRect(0, 0, rulerWidth, rulerWidth)
        ctx.fillStyle ="black"
        ctx.fillText("CM", rulerWidth/6, rulerWidth/3 * 2)

    }

    Flickable{
        id: album
        topMargin: 50
        leftMargin: 50
        anchors.fill: parent
        contentHeight: 0
        contentWidth: 0
        ScrollBar.horizontal:ScrollBar{}
        ScrollBar.vertical:ScrollBar{}

        onMovementEnded: {//此处不含滚轮的事件
            console.log("ruler repaint")
            console.log('contentX: ', album.contentX)
            console.log('contentY: ', album.contentY)
            console.log('contentWidth: ', album.contentWidth)
            console.log('contentHeight: ', album.contentHeight)
            console.log('width: ', album.width)
            console.log('height: ', album.height)
            canvas_ruler.requestPaint()
        }

        DcmImageView{
            id: dcm_image
            anchors.fill: parent
            colorLow: color_window.first.value
            colorHigh: color_window.second.value

            onColorWindowChanged:{
                color_window.first.value = dcm_image.colorLow
                color_window.second.value = dcm_image.colorHigh
            }
            MouseArea{
                id: calibrationArea
                enabled: false
                anchors.fill:parent
                preventStealing: true
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                Line{
                    id: selectCalibrationLine
                    visible: false
                    point1: Qt.point(100,400)
                    point2: Qt.point(100,400)
                    lineWidth: 4
                }
                onPositionChanged: {
                    if(mouse.buttons === Qt.LeftButton){
                        var width = mouse.x - selectCalibrationLine.point1.x
                        var height = mouse.y - selectCalibrationLine.point1.y
                        selectCalibrationLine.point2 = Qt.point(mouse.x, mouse.y)
                    }
                }
                onPressed: {
                    if(mouse.button === Qt.LeftButton){
                        console.log("pressed")
                        selectCalibrationLine.point1 = Qt.point(mouse.x, mouse.y)
                        selectCalibrationLine.visible = true
                    }
                }
                onReleased: {
                    if(mouse.button === Qt.LeftButton){
                        //mouse.accepted = false
                        console.log("released")
                        selectCalibrationLine.visible = false
                        dcm_image.countMeasureScale(mouse.x / dcm_image.getZoom(), selectCalibrationLine.point1.x/ dcm_image.getZoom(), mouse.y/ dcm_image.getZoom(), selectCalibrationLine.point1.y/ dcm_image.getZoom())
                        calibrationArea.enabled = false
                        calibrationButton.enabled = false
                        calibrationButton.highlighted = false
                        lengthMeasureButton.enabled = true
                        lengthMeasureButton.highlighted = true
                        measureArea.enabled = true
                        selectMeasureArea.visible = false
                        selectMeasureLine.visible = true
                        selectMeasureLine.point1 = Qt.point(0,0)
                        selectMeasureLine.point2 = Qt.point(0,0)
                        calibrationArea.enabled = false
                        calibrationButton.enabled = false
                        areaMeasureButton.enabled = false
                        canvas_ruler.requestPaint()
                    }
                }

            }
            MouseArea{
                id: measureArea
                enabled: false
                anchors.fill:parent
                preventStealing: true
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                Rectangle{
                    id:selectMeasureArea
                    color:"transparent"
                    border.color:"red"
                    border.width: 1
                    visible:false
                    height: 50
                    transform: Scale{
                        id: measureAreaScale
                        origin.x:0
                        origin.y:0
                        xScale:1
                        yScale:1
                    }
                }
                Line{
                    id: selectMeasureLine
                    visible: false
                    point1: Qt.point(100,400)
                    point2: Qt.point(100,400)
                    lineWidth: 4
                }

                onPositionChanged: {
                    if(mouse.buttons === Qt.LeftButton){
                        var width = mouse.x-selectMeasureArea.x
                        var height = mouse.y - selectMeasureArea.y
                        if(width >= 0){
                            selectMeasureArea.width = width
                            measureAreaScale.xScale = 1
                        }else{
                            selectMeasureArea.width = -width
                            measureAreaScale.xScale = -1
                        }
                        if(height >= 0){
                            selectMeasureArea.height = height
                            measureAreaScale.yScale = 1
                        }else{
                            selectMeasureArea.height = -height
                            measureAreaScale.yScale = -1
                        }
                        selectMeasureLine.point2 = Qt.point(mouse.x, mouse.y)
                    }
                    var len = dcm_image.countLength(mouse.x/ dcm_image.getZoom(), selectMeasureArea.x/ dcm_image.getZoom(), mouse.y/ dcm_image.getZoom(), selectMeasureArea.y/ dcm_image.getZoom())
                    var angle = dcm_image.countAngle(mouse.x/ dcm_image.getZoom(), selectMeasureArea.x/ dcm_image.getZoom(), mouse.y/ dcm_image.getZoom(), selectMeasureArea.y/ dcm_image.getZoom())
                    var area = dcm_image.countArea(mouse.x/ dcm_image.getZoom(), selectMeasureArea.x/ dcm_image.getZoom(), mouse.y/ dcm_image.getZoom(), selectMeasureArea.y/ dcm_image.getZoom())
                    if(selectMeasureLine.visible == true){
                        measureLenResult.text = "长度：" + len + "mm，角度：" + angle + "度"
                    }else{
                        measureLenResult.text = "面积" + area
                    }
                }
                onPressed: {
                    if(mouse.button === Qt.LeftButton){
                        console.log("pressed")
                        selectMeasureArea.x = mouse.x
                        selectMeasureArea.y = mouse.y
                        selectMeasureArea.width = 0
                        selectMeasureArea.height = 0
                        selectMeasureLine.point1 = Qt.point(mouse.x, mouse.y)
                    }
                }
                onReleased: {
                    if(mouse.button === Qt.LeftButton){
                        //mouse.accepted = false
                        console.log("released")
                    }
                }

            }

            MouseArea{
                id: snrArea
                enabled: false
                anchors.fill:parent
                preventStealing: true
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                Rectangle{
                    id:selectSNRArea
                    color:"transparent"
                    border.color:"red"
                    border.width: 1
                    visible:false
                    height: 50
                    transform: Scale{
                        id: snrAreaScale
                        origin.x:0
                        origin.y:0
                        xScale:1
                        yScale:1
                    }
                }

                onPositionChanged: {
                    if(mouse.buttons === Qt.LeftButton){
                        var width = mouse.x-selectSNRArea.x
                        var height = mouse.y - selectSNRArea.y
                        if(width >= 0){
                            selectSNRArea.width = width
                            snrAreaScale.xScale = 1
                        }else{
                            selectSNRArea.width = -width
                            snrAreaScale.xScale = -1
                        }
                        if(height >= 0){
                            selectSNRArea.height = height
                            snrAreaScale.yScale = 1
                        }else{
                            selectSNRArea.height = -height
                            snrAreaScale.yScale = -1
                        }
                    }
                }
                onPressed: {
                    if(mouse.button === Qt.LeftButton){
                        console.log("pressed")
                        selectSNRArea.x = mouse.x
                        selectSNRArea.y = mouse.y
                        selectSNRArea.width = 0
                        selectSNRArea.height = 0
                    }
                }
                onReleased: {
                    if(mouse.button === Qt.LeftButton){
                        //mouse.accepted = false
                        var ave = dcm_image.countAve(mouse.x/ dcm_image.getZoom(), selectSNRArea.x/ dcm_image.getZoom(), mouse.y/ dcm_image.getZoom(), selectSNRArea.y/ dcm_image.getZoom())
                        var sigma = dcm_image.countSigma(ave, mouse.x/ dcm_image.getZoom(), selectSNRArea.x/ dcm_image.getZoom(), mouse.y/ dcm_image.getZoom(), selectSNRArea.y/ dcm_image.getZoom())
                        var snr = dcm_image.countSNR(ave, sigma)
                        snrMean.text = ave
                        snrStd.text = sigma
                        snrValue.text = snr

                        subWindowSNR.visible = true

                        console.log(snr)
                        console.log("released")
                    }
                }

            }
            MouseArea{
                id: dwArea
                enabled: false
                anchors.fill:parent
                preventStealing: true
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                Line{
                    id: selectDWLine
                    visible: false
                    point1: Qt.point(100,400)
                    point2: Qt.point(100,400)
                    lineWidth: 4
                }

                onPositionChanged: {
                    if(mouse.buttons === Qt.LeftButton){
                        selectDWLine.point2 = Qt.point(mouse.x, mouse.y)
                    }
                }
                onPressed: {
                    if(mouse.button === Qt.LeftButton){
                        console.log("pressed")
                        selectDWLine.point1 = Qt.point(mouse.x, mouse.y)
                    }
                }
                onReleased: {
                    if(mouse.button === Qt.LeftButton){
                        //mouse.accepted = false
                        var intensity = dcm_image.countDwArray(mouse.x/ dcm_image.getZoom(), selectDWLine.point1.x/ dcm_image.getZoom(), mouse.y/ dcm_image.getZoom(), selectDWLine.point1.y/ dcm_image.getZoom())

                        subWindowDw.visible = true

                        intensityLineSeries.removePoints(0, intensityLineSeries.count)
                        var i
                        var minIntensity = 10000000
                        var maxIntensity = 0
                        for(i in intensity){
                            console.log(i)
                            console.log(intensity[i])
                            intensityLineSeries.append(i, intensity[i])
                            if(intensity[i] > maxIntensity){
                                maxIntensity = intensity[i]
                            }
                            if(intensity[i] < minIntensity){
                                minIntensity = intensity[i]
                            }
                        }
                        intensityLineSeries.axisX.min = 0
                        intensityLineSeries.axisX.max = i
                        intensityLineSeries.axisY.min = minIntensity
                        intensityLineSeries.axisY.max = maxIntensity

                        doubleWall_button.highlighted = false
                        dwArea.enabled = false
                        selectDWLine.visible = false

                        console.log(intensity)

                        var dip = dcm_image.countDw(mouse.x/ dcm_image.getZoom(), selectDWLine.point1.x/ dcm_image.getZoom(), mouse.y/ dcm_image.getZoom(), selectDWLine.point1.y/ dcm_image.getZoom())
                        for(i in dip){
                            console.log(i)
                            console.log(dip[i])
                            if(i < 13){
                                if(dip[i]<0.2){
                                    var dwRes = Number(i)+1
                                    break
                                }
                            }
                        }
                        dw1.text = dip[0].toFixed(2)
                        dwc1.x = dwResRec.width*dip[13]/intensity.length
                        dw2.text = dip[1].toFixed(2)
                        dwc2.x = dwResRec.width*dip[14]/intensity.length
                        dw3.text = dip[2].toFixed(2)
                        dwc3.x = dwResRec.width*dip[15]/intensity.length
                        dw4.text = dip[3].toFixed(2)
                        dwc4.x = dwResRec.width*dip[16]/intensity.length
                        dw5.text = dip[4].toFixed(2)
                        dwc5.x = dwResRec.width*dip[17]/intensity.length
                        dw6.text = dip[5].toFixed(2)
                        dwc6.x = dwResRec.width*dip[18]/intensity.length
                        dw7.text = dip[6].toFixed(2)
                        dwc7.x = dwResRec.width*dip[19]/intensity.length
                        dw8.text = dip[7].toFixed(2)
                        dwc8.x = dwResRec.width*dip[20]/intensity.length
                        dw9.text = dip[8].toFixed(2)
                        dwc9.x = dwResRec.width*dip[21]/intensity.length
                        dw10.text = dip[9].toFixed(2)
                        dwc10.x = dwResRec.width*dip[22]/intensity.length
                        dw11.text = dip[10].toFixed(2)
                        dwc11.x = dwResRec.width*dip[23]/intensity.length
                        dw12.text = dip[11].toFixed(2)
                        dwc12.x = dwResRec.width*dip[24]/intensity.length
                        dw13.text = dip[12].toFixed(2)
                        dwc13.x = dwResRec.width*dip[25]/intensity.length
                        dwResText.text = "第一组不大于20%的：DW" + dwRes
                        console.log("released")
                    }
                }

            }

            MouseArea{
                id: right_button_func
                enabled: false
                Rectangle{
                    id:select_area
                    color:"transparent"
                    border.color:"red"
                    border.width: 1
                    visible:false
                    height: 50
                    transform: Scale{
                        id: select_scale
                        origin.x:0
                        origin.y:0
                        xScale:1
                        yScale:1
                    }
                }

                anchors.fill:parent
                preventStealing: true
                //propagateComposedEvents: true
                hoverEnabled: true
                acceptedButtons: Qt.RightButton
                onWheel: {
                    var angle = wheel.angleDelta.y
                    if(angle > 0){
                        dcm_image.zoomIn()
                        album.fitImage(dcm_image.disHeight,dcm_image.disWidth)
                    }
                    if(angle < 0){
                        dcm_image.zoomOut()
                        album.fitImage(dcm_image.disHeight,dcm_image.disWidth)
                    }
                }
                onPositionChanged: {
                    pixel_info.text = " 鼠标位置：" + (mouse.x/ dcm_image.getZoom()).toFixed(0) + "*" + (mouse.y/ dcm_image.getZoom()).toFixed(0) + " 像素值：" + dcm_image.getPixelValue(mouse.x, mouse.y);
                    if(mouse.buttons === Qt.RightButton){
                        var width = mouse.x-select_area.x
                        var height = mouse.y - select_area.y
                        if(width >= 0){
                            select_area.width = width
                            select_scale.xScale = 1
                        }else{
                            select_area.width = -width
                            select_scale.xScale = -1
                        }
                        if(height >= 0){
                            select_area.height = height
                            select_scale.yScale = 1
                        }else{
                            select_area.height = -height
                            select_scale.yScale = -1
                        }
                    }
                }

                onPressed: {
                    if(mouse.button === Qt.RightButton){
                        console.log("pressed")
                        select_area.x = mouse.x
                        select_area.y = mouse.y
                        select_area.width = 0
                        select_area.height = 0
                        select_area.visible = true
                    }
                }
                onReleased: {
                    if(mouse.button === Qt.RightButton){
                        //mouse.accepted = false
                        console.log("released")
                        select_area.visible = false
                        dcm_image.changeColorWindow(select_area.x, select_area.y, mouse.x, mouse.y)
                    }
                }

            }
        }

        function openFile(url){
            LocalImage.loadDcmImage(url)
            dcm_image.loadCurImage(LocalImage)
            dcm_image.updateImage()
            album.fitImage(dcm_image.disHeight,dcm_image.disWidth)
            image_info.text = "图像尺寸:" + dcm_image.imgWidth + "*"  + dcm_image.imgHeight
            dcm_image.realtime = false
            file_nav.close();
        }

        function fitImage(height, width){
            console.log(height, width)
            album.contentHeight = height;
            album.contentWidth = width;
            right_button_func.enabled = true
            canvas_ruler.requestPaint()
        }
    }


    Column{
        x: 100
        y: 100
        spacing:10
        Button{
            text:"文件"
            height: 20
            width: 50
            onClicked: {
                if(file_nav.visible){
                    file_nav.close()
                }else{
                    file_nav.open()
                }
            }
        }

        Button{
            text:"菜单"
            height: 20
            width: 50
            onClicked: {
                if(menu_nav.visible){
                    menu_nav.close()
                }else{
                    menu_nav.open()
                }
            }
        }


    }

    Window {
        id:             subWindowSaveInfo
        width:          1000
        height:         800
        visible:        false

        Column{
            spacing: 50
            Row{
                Text{
                    text:"是否自动保存:"
                }
                ComboBox{
                    id: bAutoSave
                    width: 300
                    currentIndex: 0

                    model: ["不自动保存", "自动保存"]
                }
            }
            Row{
                Text{
                    text:"文件名格式:"
                }
                TextField{
                    id: fileNameInput
                    text: "项目1"
                    selectByMouse: true

                    onFocusChanged: {
                        if(focus){
                            selectAll()
                        }
                    }

                    onTextChanged:{
                        if(!acceptableInput){
                            text = "项目1"
                        }
                    }
                }
                Text{
                    text:"+时间序列数:"
                }
            }
            Row{
                Button{
                    text:"保存位置设置"
                    height:20
                    onClicked: {
                        fileDialogSavePos.open()
                    }
                }
                TextField{
                    id: fileSavePos
                    text: "E:\\dcmSave"
                    selectByMouse: true

                    onFocusChanged: {
                        if(focus){
                            selectAll()
                        }
                    }

                    onTextChanged:{
                        if(!acceptableInput){
                            text = "E:\\dcmSave"
                        }
                    }
                }
            }
            Button{
                text: "应用设置"
                onClicked: {
                    var str1 = fileSavePos.text
                    var str2 = fileNameInput.text
                    var filePath = str1 + "\\" + str2
                    console.log(filePath)
                    XRayDetector.setFilePath(filePath)
                    console.log(XRayDetector.getFilePath())

                    XRayDetector.test()
                }
            }
        }
    }
    Window {
        id:             subWindowSNR
        width:          1000
        height:         800
        visible:        false

        Column{
            spacing: 50
            Row{
                Text{
                    text:"均值: "
                }
                Text{
                    id: snrMean
                    text:""
                }
            }
            Row{
                Text{
                    text:"标准差: "
                }
                Text{
                    id: snrStd
                    text:""
                }
            }
            Row{
                Text{
                    text:"测量信噪比: "
                }
                Text{
                    id: snrValue
                    text:""
                }
            }
            Row{
                Text{
                    text:"图像分辨率（像元尺寸）: "
                }
                TextField{
                    id: snrResolution
                    text: "100"
                    selectByMouse: true

                    onFocusChanged: {
                        if(focus){
                            selectAll()
                        }
                    }

                    onTextChanged:{
                        if(!acceptableInput){
                            text = "E:\\dcmSave"
                        }
                    }
                }
                Button{
                    text: "计算"
                    onClicked: {
                        var num = snrValue.text * 88.6 / snrResolution.text
                        snrNorm.text = num.toFixed(2)
                    }
                }
            }
            Row{
                Text{
                    text: "归一化信噪比: "
                }
                Text{
                    id: snrNorm
                    text: ""
                }
            }
        }
    }
    Window {
        id:             subWindowDw
        width:          1000
        height:         800
        visible:        false

        Rectangle{
            id: recChart
            width: parent.width
//            height: parent.height-recChartText.height-recDwRes.height
            height: 700
            anchors.top: parent.top
            anchors.bottom: recChartText.top
            ChartView{
                title: "曲线分析"
                anchors.fill: parent
                antialiasing: true
                LineSeries{
                    id: intensityLineSeries
                    name: "像素值"
                }
            }
        }

        Rectangle{
            id: recChartText
            width: parent.width
            height: 50
            anchors.top: recChart.bottom
            anchors.bottom: recDwRes.top
            Row{
                x: 100
                width: parent.width-200
                Rectangle{
                    id: dwResRec
                    width: parent.width
                    Column{
                        id: dwc1
                        Text{
                            text:"DW1"
                        }
                        Text{
                            id: dw1
                            text:"DW1"
                        }
                    }
                    Column{
                        id: dwc2
                        Text{
                            text:"DW2"
                        }
                        Text{
                            id: dw2
                            text:"DW2"
                        }
                    }
                    Column{
                        id: dwc3
                        Text{
                            text:"DW3"
                        }
                        Text{
                            id: dw3
                            text:"DW3"
                        }
                    }
                    Column{
                        id: dwc4
                        Text{
                            text:"DW4"
                        }
                        Text{
                            id: dw4
                            text:"DW4"
                        }
                    }
                    Column{
                        id: dwc5
                        Text{
                            text:"DW5"
                        }
                        Text{
                            id: dw5
                            text:"DW5"
                        }
                    }
                    Column{
                        id: dwc6
                        Text{
                            text:"DW6"
                        }
                        Text{
                            id: dw6
                            text:"DW6"
                        }
                    }
                    Column{
                        id: dwc7
                        Text{
                            text:"DW7"
                        }
                        Text{
                            id: dw7
                            text:"DW7"
                        }
                    }
                    Column{
                        id: dwc8
                        Text{
                            text:"DW8"
                        }
                        Text{
                            id: dw8
                            text:"DW8"
                        }
                    }
                    Column{
                        id: dwc9
                        Text{
                            text:"DW9"
                        }
                        Text{
                            id: dw9
                            text:"DW9"
                        }
                    }
                    Column{
                        id: dwc10
                        Text{
                            text:"DW10"
                        }
                        Text{
                            id: dw10
                            text:"DW10"
                        }
                    }
                    Column{
                        id: dwc11
                        Text{
                            text:"DW11"
                        }
                        Text{
                            id: dw11
                            text:"DW11"
                        }
                    }
                    Column{
                        id: dwc12
                        Text{
                            text:"DW12"
                        }
                        Text{
                            id: dw12
                            text:"DW12"
                        }
                    }
                    Column{
                        id: dwc13
                        Text{
                            text:"DW13"
                        }
                        Text{
                            id: dw13
                            text:"DW13"
                        }
                    }
                }
            }
        }
        Rectangle{
            id: recDwRes
            width: parent.width
            height: 50
//            anchors.top: recChartText.bottom
            anchors.bottom: parent.bottom
            Text{
                id: dwResText
                text: "第一组不大于20%的"
            }
        }



//        Column{
//            spacing: 50

//            Row{
//                Text{
//                    text:"DW1: "
//                }
//                Text{
//                    id: dw1
//                    text:""
//                }
//            }
//        }
    }

    Window {
        id:             subWindow
        width:          1000
        height:         800
        visible:        false

        Column{
            spacing: 50
            Row{
                visible: false
                Text{
                    text:"是否显示水印:"
                }
                ComboBox{
                    id: bAddInfo
                    width: 300
                    currentIndex: 0

                    model: ["显示水印", "不显示水印"]
                }
            }
            Row{
                Text{
                    text:"设置水印位置 X坐标: "
                }
                TextField{
                    id: positionX
                    text: "10"
                    selectByMouse: true

                    onFocusChanged: {
                        if(focus){
                            selectAll()
                        }
                    }

                    onTextChanged:{
                        if(!acceptableInput){
                            text = "10"
                        }
                    }
                }
                Text{
                    text:" Y坐标: "
                }
                TextField{
                    id: positionY
                    text: "10"
                    selectByMouse: true

                    onFocusChanged: {
                        if(focus){
                            selectAll()
                        }
                    }

                    onTextChanged:{
                        if(!acceptableInput){
                            text = "10"
                        }
                    }
                }
            }
            Row{
                Text{
                    text:"设置文字大小: "
                }
                TextField{
                    id: fontSize
                    text: "30"
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
                    text:"设置文字字体: "
                }
                ComboBox{
                    id: fontType
                    width: 300
                    currentIndex: 0

                    model: ["楷体", "微软雅黑", "华文行楷", "华文宋体", "隶书", "Arial", "Times New Roman"]
                }
            }
            Row{
                Text{
                    text:"设置文字信息: "
                }
                Column{
                    Row{
                        TextField{
                            id: imageInfoPrj
                            text: "项目："
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
                        TextField{
                            id: imageInfoPrjDescription
                            text: "项目描述"
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
                        TextField{
                            id: imageInfoPeople
                            text: "检验人："
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
                        TextField{
                            id: imageInfoPeopleDescription
                            text: "检验人描述"
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
                        TextField{
                            id: imageInfoCheckList
                            text: "检验项目："
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
                        TextField{
                            id: imageInfoCheckListDescription
                            text: "检验项目描述"
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
                        TextField{
                            id: imageInfoSize
                            text: "尺寸："
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
                        TextField{
                            id: imageInfoSizeDescription
                            text: "尺寸描述"
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
                        TextField{
                            id: imageInfoOthers
                            text: "备注："
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
                        TextField{
                            id: imageInfoOthersDescription
                            text: "备注描述 "
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


                }


            }
            Row{
                spacing: 50
                Button{
                    text: "添加水印"
                    onClicked: {
                        var str1 = imageInfoPrj.text
                        var str2 = imageInfoPrjDescription.text
                        var str3 = imageInfoPeople.text
                        var str4 = imageInfoPeopleDescription.text
                        var str5 = imageInfoCheckList.text
                        var str6 = imageInfoCheckListDescription.text
                        var str7 = imageInfoSize.text
                        var str8 = imageInfoSizeDescription.text
                        var str9 = imageInfoOthers.text
                        var str10 = imageInfoOthersDescription.text
                        var str = str1 + str2 + "\n" + str3 + str4 + "\n" + str5 + str6 + "\n" + str7 + str8 + "\n" + str9 + str10
                        dcm_image.setImgInfo(bAddInfo.currentText, str, positionX.text, positionY.text, fontSize.text, fontType.currentText)
                    }
                }
                Button{
                    text: "清除水印"
                    onClicked: {
                        dcm_image.clearImgInfo()
                    }
                }

            }


        }
    }


    Rectangle{
        width:parent.width
        height:30
        color:"transparent"
        anchors.bottom:parent.bottom
        anchors.left: parent.left

        Column{
            x: 60
            Text{
                id: measureLenResult
                text: ""
                color:"yellow"
                font.pointSize: 12
            }
            Row{
                Label{
                    id:image_info
                    text:"图像尺寸:"
                    color:"yellow"
                    font.pointSize: 12
                }
                Label{
                    id:pixel_info
                    text:"像素值:"
                    color:"yellow"
                    font.pointSize: 12
                }
            }
        }
        RangeSlider{
            id:color_window
            stepSize: 1
            anchors.right: parent.right
            snapMode: "SnapAlways"
            first.handle: Rectangle{
                x: color_window.leftPadding + color_window.first.visualPosition * (color_window.availableWidth - width)
                y: color_window.topPadding + color_window.availableHeight / 2 - height / 2
                width:20
                height:20
                radius: width/2
                color: color_window.pressed ? "#f0f0f0" : "#f6f6f6"
                border.color: "#bdbebf"
                Label{
                    y:parent.y-10
                    text:color_window.first.value
                }
            }
            second.handle: Rectangle{
                x: color_window.leftPadding + color_window.second.visualPosition * (color_window.availableWidth - width)
                y: color_window.topPadding + color_window.availableHeight / 2 - height / 2
                width:20
                height:20
                radius: width/2
                color: color_window.pressed ? "#f0f0f0" : "#f6f6f6"
                border.color: "#bdbebf"
                Label{
                    y:parent.y-10
                    text:color_window.second.value
                }
            }
            from:0
            to:65535
            first.value: 0
            second.value : 40000
            first.onValueChanged: {
                if(first.value >= second.value){
                    first.value = second.value - 255
                }
                dcm_image.colorLow = first.value
            }
            second.onValueChanged:{
                if(second.value <= first.value){
                    second.value = first.value + 255
                }
                dcm_image.colorHigh = second.value
            }

        }
    }
}

