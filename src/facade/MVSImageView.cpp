#include "MVSImageView.h"

MVSImageView::MVSImageView(QQuickItem  *parent) : QQuickPaintedItem(parent)
{
    m_pBufForDriverLastTime = nullptr;
}

QImage MVSImageView::generateQImage(int colorLow, int colorHigh){
    qDebug()<<"generating qImage";


//    MVSCamera().getImageBuffer();//不是一个实例
    m_qImage = QImage(MVSCamera::m_pBufForDriver, MVSCamera::m_nWidth, MVSCamera::m_nHeight, QImage::Format_RGB888);

//    m_qImage.load("C:/Users/Lenovo/Desktop/q.jpg");
    qDebug()<< "qImage width: " << m_qImage.width();
    update();

    return m_qImage;
}

void MVSImageView::paint(QPainter *painter)
{
    MVSCamera::getImageBuffer();
//    if(MVSCamera::m_nHeight == 0){
//        qDebug("empty image pointer");
////        MVSCamera::m_pBufForDriver = m_pBufForDriverLastTime;
////        qDebug() << "bufferSize: " << MVSCamera::m_nBufSizeForDriver;
//        memcpy(MVSCamera::m_pBufForDriver, m_pBufForDriverLastTime, MVSCamera::m_nBufSizeForDriver);
//    } else {
////        qDebug() << "bufferSize: " << MVSCamera::m_nBufSizeForDriver;
//        memcpy(m_pBufForDriverLastTime, MVSCamera::m_pBufForDriver, MVSCamera::m_nBufSizeForDriver);
//    }
    m_qImage = generateQImage(0, 255);
    qDebug() << "start paint";
    QImage scaled;
    scaled = m_qImage.scaled(
                MVSCamera::m_nWidth,
                MVSCamera::m_nHeight,
                Qt::KeepAspectRatioByExpanding
                );
    painter->drawImage(QPointF(0,0), scaled);
//    painter->drawImage(QPointF(0,0), m_qImage);
}
