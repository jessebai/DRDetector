//dcm fileformat read and write functions
//all static methods, cannot be initiated.

#ifndef DCMPROCESS_H
#define DCMPROCESS_H

#include <QImage>
#include<qdebug.h>

#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>

using namespace std;
class DcmProcess
{
public:
    DcmProcess();

    static DicomImage* loadDcmImage(const char* filename);

    static void saveAsDcmFile(unsigned short* pImage, int nWidth, int nHeight, QString filePath);

    static void saveAsDcmFileWithPath(unsigned short* pImage, int nWidth, int nHeight, string filePath);


};

#endif // DCMPROCESS_H
