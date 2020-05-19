//image data received from DRdetector.
//this is used as a singleton class
//the realtime image should notify the scheduler that image has finished processing
//to continue the pipeline.
//the process results are buffered in scheduler, which will forward to the plc regularly.

#ifndef REALTIMEIMAGE_H
#define REALTIMEIMAGE_H

#include <QObject>

#include "ImageData.h"

class RealtimeImage: public ImageData
{
    Q_OBJECT

signals:
    void imgProcComplete(QByteArray count, QByteArray results);
public:
    RealtimeImage();

    //called by Xray detector when new image data is available
    Q_INVOKABLE void loadRealtimeImage(long frmAddr, int nWidth, int nHeight);

    Q_INVOKABLE void test();

};

#endif // REALTIMEIMAGE_H
