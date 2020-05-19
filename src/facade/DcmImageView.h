#ifndef DCMIMAGEVIEW_H
#define DCMIMAGEVIEW_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QVector>

#include <algorithm>

#include "src/data/DcmProcess.h"
#include "src/data/ImageData.h"

class DcmImageView:public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage)
    Q_PROPERTY(int colorLow READ low WRITE setLow)
    Q_PROPERTY(int colorHigh READ high WRITE setHigh)
    Q_PROPERTY(int imgHeight READ imgHeight)
    Q_PROPERTY(int imgWidth READ imgWidth)
    Q_PROPERTY(int disHeight READ disHeight)
    Q_PROPERTY(int disWidth READ disWidth)
    Q_PROPERTY(bool realtime MEMBER m_bRealTimeToggle NOTIFY realTimeChanged)
    Q_PROPERTY(ImageData* imageData MEMBER m_curImgData)
public:
    //basic functions
    explicit DcmImageView(QQuickItem  *parent = nullptr);
    void paint(QPainter *painter);
    Q_INVOKABLE void updateImage();
    Q_INVOKABLE void drawImage();

    //static methods, required for QImage, otherwise will stack overflow
    static void freeData(void* data);

    QImage generateQImage(int colorLow, int colorHigh);


    //load current image data instance. Can be pointer to local ot realtime image
    Q_INVOKABLE void loadCurImage(ImageData* curImg);

    //save current image
    Q_INVOKABLE void saveCurImage(QString filePath, int colorLow, int colorHigh);

    //set image info parameter
    Q_INVOKABLE void setImgInfo(QString m_bAddInfo, QString str, QString positionX, QString positionY, QString fontSize, QString fontType);
    // clear image info
    Q_INVOKABLE void clearImgInfo();

    //image show
    void initZoom();
    Q_INVOKABLE void zoomIn();
    Q_INVOKABLE void zoomOut();

    //mouse pos to image coordinates convertion
    int posToCoor(double pos);

    //image info
    Q_INVOKABLE int getPixelValue(double x, double y);

    //color window change
    Q_INVOKABLE void changeColorWindow(double x1, double y1, double x2, double y2);

    //property getter setter
    QImage image() {return m_activeImage; }
    void setImage(QImage image) { m_activeImage = image; update(); }
    int low() {return m_nColorLow;}
    void setLow(int newLow);
    int high() {return m_nColorHigh;}
    void setHigh(int newHigh);
    int imgHeight() {return m_nRawImgHeight;}
    int imgWidth() {return m_nRawImgWidth;}
    int disHeight() {return static_cast<int>(imgHeight() * m_dZoom); }
    int disWidth() {return static_cast<int>(imgWidth() * m_dZoom); }

    //get image process result
    Mat p2mat(void* data);

    //image measure
    Q_INVOKABLE void setMeasureScale(double scale);
    Q_INVOKABLE double getMeasureScale();
    Q_INVOKABLE void countMeasureScale(double x1, double x2, double y1, double y2);
    Q_INVOKABLE QString countLength(double x1, double x2, double y1, double y2);
    Q_INVOKABLE QString countAngle(double x1, double x2, double y1, double y2);
    Q_INVOKABLE QString countArea(double x1, double x2, double y1, double y2);
    Q_INVOKABLE double getZoom();
    Q_INVOKABLE QString countAve(double x1, double x2, double y1, double y2);
    Q_INVOKABLE QString countSigma(double ave, double x1, double x2, double y1, double y2);
    Q_INVOKABLE QString countSNR(double ave, double sigma);
    Q_INVOKABLE QVector<int> countDwArray(double x1, double x2, double y1, double y2);
    Q_INVOKABLE QVector<double> countDw(double x1, double x2, double y1, double y2);

signals:
    void realTimeChanged(bool);
    void colorWindowChanged();

private:
    //real time toggle
    //true: show real time image
    //false: show local image
    bool m_bRealTimeToggle = true;

    ImageData* m_curImgData;
    //real time image pointer
    ImageData* m_realtimeImage = new ImageData();

    //local image pointer
    ImageData* m_localImage = new ImageData();

    //raw image
    DicomImage* m_pDcmImage = nullptr;
    int m_nRawImgWidth;
    int m_nRawImgHeight;
    unsigned short* m_pRawImage;

    //display
    int m_nDisplayWidth;
    int m_nDisplayHeight;
    QImage m_activeImage;   //the displayed image with depth 8
    QRectF m_displayArea;
    QImage m_activeQImage;

    //color scale
    int m_nColorLow = 0;
    int m_nColorHigh = 40000;
    double m_dZoom= 1.0;

    //process result
    Mat m_MOrg;

    //add info in the picture
    bool m_bAddInfo = false;
    QString m_sInfo;
    int m_nPositionX = 0;
    int m_nPositionY = 0;
    int m_nFontSize = 30;
    const char* m_cFontType;

    //image measurement
    double m_dMeasureScale = 0.1; // realLength/pixelLength
    double m_dFixedLength = 50; // fixed realLength
};

#endif // DCMIMAGEVIEW_H
