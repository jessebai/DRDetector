#ifndef MVSIMAGEVIEW_H
#define MVSIMAGEVIEW_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QDebug>

#include "src/data/DcmProcess.h"
#include "src/data/ImgProcess.h"
#include "src/data/ImageData.h"
#include "src/device/MVSCamera.h"

class MVSImageView:public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit MVSImageView(QQuickItem  *parent = nullptr);

    //画图
    Q_INVOKABLE QImage generateQImage(int low, int high);
    void paint(QPainter *painter);


//    int m_nWidth;
//    int m_nHeight;
    QImage m_qImage;

    //缓存的上一次不为空的图片
    unsigned char*  m_pBufForDriverLastTime;


signals:


private:

};

#endif // DCMIMAGEVIEW_H
