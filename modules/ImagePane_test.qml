import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2
import Qt.labs.folderlistmodel 2.2


import MVSImageView 1.0
import data.LocalImage 1.0
import data.RealtimeImage 1.0

Pane{
    id: image_view_test
    background: Rectangle{
        color:"grey"
    }
    //the slot to receive signal from detector
    function newRealTimeImage(frmAddr, nWidth, nHeight){
        dcm_image.newRealTimeImage(frmAddr, nWidth, nHeight)
    }

    Flickable{
        id: album
        anchors.fill: parent
        contentHeight: height
        contentWidth: width
        ScrollBar.horizontal:ScrollBar{}
        ScrollBar.vertical:ScrollBar{}


        MVSImageView{
            id: mvs_image
            anchors.fill:parent
        }


        function fitImage(height, width){
            console.log(height, width)
            album.contentHeight = height;
            album.contentWidth = width;
            right_button_func.enabled = true
        }
    }
}

