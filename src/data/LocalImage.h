#ifndef LOCALIMAGE_H
#define LOCALIMAGE_H

#include <QObject>
#include <QUrl>
#include "ImageData.h"

#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>

class LocalImage :public ImageData
{
    Q_OBJECT
public:
    explicit LocalImage();


    Q_INVOKABLE void loadDcmImage(QUrl filename);


signals:

public slots:

private:
    DicomImage *m_pDcmImage;

};

#endif // LOCALIMAGE_H
