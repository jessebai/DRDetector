#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QImage>
#include <QObject>
#include <QByteArray>
#include <stdlib.h>
#include <stdio.h>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QTextCodec>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

#include "puttext.h"

using namespace std;
using namespace cv;

class ImageData: public QObject
{
    Q_OBJECT
public:
    ImageData(QObject* parent = nullptr);

    //static methods, required for QImage, otherwise will stack overflow
    static void freeData(void* data);

    //new image from detector
    void updateImage(unsigned short* frmAddr, int width, int height);


    Mat p2mat(void* data);

    //generate QImage for raw image
    Q_INVOKABLE QImage genRawQImage(int colorLow, int colorHigh);

    //add str in image (/n represents enter)
    Q_INVOKABLE void addStr(QString str, int x, int y, int fontSize, const char* fontFace);

    //getter
    int nRawImgWidth();

    int nRawImgHeight();

    unsigned short *pRawImage() const;

    //setter
    void setNRawImgWidth(int nRawImgWidth);

    void setNRawImgHeight(int nRawImgHeight);

    void setPRawImage(unsigned short *pRawImage);

    void setImageCopy();

    void copyImage();

private:
    //raw data
    int m_nRawImgWidth;
    int m_nRawImgHeight;
    unsigned short* m_pRawImage;
    unsigned short* m_pRawImageCopy;
};

#endif // IMAGEDATA_H
