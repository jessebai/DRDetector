#include "RealtimeImage.h"

RealtimeImage::RealtimeImage():ImageData()
{

}

void RealtimeImage::loadRealtimeImage(long frmAddr, int nWidth, int nHeight){
    setNRawImgHeight(nHeight);
    setNRawImgWidth(nWidth);
    setPRawImage(reinterpret_cast<unsigned short*>(frmAddr));
    QByteArray count = "000";
    QByteArray results;
    count.append(QByteArray().setNum(results.size() + 1));

    emit imgProcComplete(count, results);
}


void RealtimeImage::test(){
    qDebug()<<"20181124";
    emit imgProcComplete("0004", "0001000200030004");
}
