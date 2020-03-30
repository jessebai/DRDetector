#include "ImageData.h"

ImageData::ImageData(QObject* parent):QObject(parent)
{

}

void ImageData::freeData(void* data){
    free(data);
}

void ImageData::updateImage(unsigned short* frmAddr, int width, int height){
    m_pRawImage = frmAddr;
    qDebug()<<width;
    m_nRawImgHeight = height;
    m_nRawImgWidth = width;
}

Mat ImageData::p2mat(void* data){
    return Mat(m_nRawImgHeight,m_nRawImgWidth,CV_16UC1,data);
}


QImage ImageData::genRawQImage(int colorLow, int colorHigh){
    qDebug()<<"generating Qimage "<<colorLow<<colorHigh;
    int windowSize = colorHigh - colorLow;
    uchar* pConvertFrame = reinterpret_cast<uchar*>(malloc(m_nRawImgHeight * m_nRawImgWidth * sizeof(uchar)));
    for(int i = 0; i < m_nRawImgHeight; i++){
        for(int j = 0; j < m_nRawImgWidth; j++){
            int nPixIdx = i * m_nRawImgWidth + j;
            unsigned short nOriColor = m_pRawImage[nPixIdx];
            if(nOriColor > colorHigh){
                pConvertFrame[nPixIdx] = 255;
            }else if(nOriColor < colorLow){
                pConvertFrame[nPixIdx] = 0;
            }else{
                pConvertFrame[nPixIdx] = static_cast<uchar>(
                                            255
                                        * static_cast<double>((nOriColor - colorLow))
                                        / windowSize
                );
            }
        }
    }
    QImage frame(pConvertFrame, m_nRawImgWidth, m_nRawImgHeight, QImage::Format_Grayscale8, freeData, pConvertFrame);
    return frame;
}

void ImageData::addStr(QString str, int x, int y, int fontSize, const char* fontFace){
//    qDebug() << "start addStr()";
//    qDebug() << str << endl << str.toStdString().data();
    Mat mRawImage = p2mat(m_pRawImage);

    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
    QString strGBK = gbk->toUnicode(str.toLocal8Bit());

    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QString strUtf8 = utf8->toUnicode(str.toUtf8());

//    qDebug() << strGBK.toStdString().data();

    Point origin;
    origin.x = x;
    origin.y = y;

//    qDebug() << fontFace;
    putTextZH(mRawImage, strGBK.toStdString().data(), origin, Scalar(0, 0, 0), fontSize, fontFace);
    m_pRawImage = reinterpret_cast<unsigned short*>(mRawImage.data);
}

int ImageData::nRawImgWidth()
{
    return m_nRawImgWidth;
}

int ImageData::nRawImgHeight()
{
    return m_nRawImgHeight;
}

unsigned short *ImageData::pRawImage() const
{
    return m_pRawImage;
}

void ImageData::setNRawImgWidth(int nRawImgWidth)
{
    m_nRawImgWidth = nRawImgWidth;
}

void ImageData::setNRawImgHeight(int nRawImgHeight)
{
    m_nRawImgHeight = nRawImgHeight;
}

void ImageData::setPRawImage(unsigned short *pRawImage)
{
    m_pRawImage = pRawImage;
    setImageCopy();
}

void ImageData::setImageCopy()
{
    qDebug()<< "setImageCopy()";
//    char* temp;
//    strcpy(temp, reinterpret_cast<const char*>(m_pRawImage));
//    m_pRawImageCopy = reinterpret_cast<unsigned short*>(temp);
    m_pRawImageCopy = reinterpret_cast<unsigned short*>(malloc(m_nRawImgWidth * m_nRawImgHeight * sizeof(unsigned short)));
    qDebug()<< "setImageCopy() memset over";
    memcpy(m_pRawImageCopy, m_pRawImage, m_nRawImgWidth * m_nRawImgHeight * sizeof(unsigned short));
}

void ImageData::copyImage()
{
    qDebug()<< "copyImage()";
//    char* temp;
//    strcpy(temp, reinterpret_cast<const char*>(m_pRawImageCopy));
//    m_pRawImage = reinterpret_cast<unsigned short*>(temp);
    memcpy(m_pRawImage, m_pRawImageCopy, m_nRawImgWidth * m_nRawImgHeight * sizeof(unsigned short));
}
