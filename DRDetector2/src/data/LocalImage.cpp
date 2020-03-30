#include "LocalImage.h"

LocalImage::LocalImage() : ImageData()
{

}

void LocalImage::loadDcmImage(QUrl fileurl){
    qDebug()<<"input file"<<fileurl;
    QString filepath = fileurl.path();
    filepath.remove(0,1);
    QByteArray bytes = filepath.toLocal8Bit();
    char* filename = bytes.data();
    qDebug()<<"final:"<<filename;
//    if(m_pDcmImage!=nullptr)
//        delete(m_pDcmImage);
    m_pDcmImage = new DicomImage(filename,CIF_UsePartialAccessToPixelData,0,0);

    if (m_pDcmImage != nullptr){
        if (m_pDcmImage->getStatus() != EIS_Normal){
            qDebug() << "Error: cannot load DICOM image (" << DicomImage::getString(m_pDcmImage->getStatus()) << ")" << endl;
        }
    }
    setNRawImgHeight(static_cast<int>(m_pDcmImage->getHeight()));
    setNRawImgWidth(static_cast<int>(m_pDcmImage->getWidth()));
    setPRawImage(reinterpret_cast<Uint16*>(const_cast<void*>((m_pDcmImage->getOutputData(16)))));
    qDebug()<<nRawImgHeight();
}
