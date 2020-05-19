#include "DcmProcess.h"

DcmProcess::DcmProcess()
{

}

DicomImage* DcmProcess::loadDcmImage(const char* filename){
    qDebug()<<"final:"<<filename;
    DicomImage *img = new DicomImage(filename,CIF_UsePartialAccessToPixelData,0,0);

    if (img != nullptr){
        if (img->getStatus() != EIS_Normal){
            qDebug() << "Error: cannot load DICOM image (" << DicomImage::getString(img->getStatus()) << ")" << endl;
        }
    }

    return img;
}

void DcmProcess::saveAsDcmFile(unsigned short* pImage, int nWidth, int nHeight, QString filePath){
    DcmFileFormat fileformat;
    OFCondition oc = fileformat.loadFile("template.dcm");

    if (oc.good()) {
        OFString strTagValue;
        if (fileformat.getDataset()->putAndInsertString(DCM_Rows, to_string(nHeight).c_str()).good())
        {
            fileformat.getDataset()->findAndGetOFString(DCM_Rows, strTagValue);
            cout << "Rows:" << strTagValue.data() << endl;
        }
        if (fileformat.getDataset()->putAndInsertString(DCM_Columns, to_string(nWidth).c_str()).good())
        {
            fileformat.getDataset()->findAndGetOFString(DCM_Columns, strTagValue);
            cout << "Columns:" << strTagValue.data() << endl;
        }

        if (fileformat.getDataset()->putAndInsertUint16Array(DCM_PixelData, pImage, static_cast<unsigned long>(nWidth * nHeight), true).good())
        {
            //按时间生成文件名
            time_t rawtime;
            time(&rawtime);
            char cc[32];
            _itoa_s(rawtime, cc, 10);
            strcat_s(cc, ".dcm");
            qDebug()<<"savedfile"<< cc;
            //char filename[32];
            //std::strftime(filename, 32, "%a, %Y-%m-%d-%H-%M-%S", rawtime);

            filePath = filePath + "\\" + QString(cc);
            fileformat.saveFile(filePath.toStdString().c_str());
        }
    } else{
        qDebug()<<oc.text();
    }
}


void DcmProcess::saveAsDcmFileWithPath(unsigned short* pImage, int nWidth, int nHeight, string filePath){
    DcmFileFormat fileformat;
    OFCondition oc = fileformat.loadFile("template.dcm");

    if (oc.good()) {
        OFString strTagValue;
        if (fileformat.getDataset()->putAndInsertString(DCM_Rows, to_string(nHeight).c_str()).good())
        {
            fileformat.getDataset()->findAndGetOFString(DCM_Rows, strTagValue);
            cout << "Rows:" << strTagValue.data() << endl;
        }
        if (fileformat.getDataset()->putAndInsertString(DCM_Columns, to_string(nWidth).c_str()).good())
        {
            fileformat.getDataset()->findAndGetOFString(DCM_Columns, strTagValue);
            cout << "Columns:" << strTagValue.data() << endl;
        }

        if (fileformat.getDataset()->putAndInsertUint16Array(DCM_PixelData, pImage, static_cast<unsigned long>(nWidth * nHeight), true).good())
        {
            fileformat.saveFile(filePath.c_str());
        }
    } else{
        qDebug() << filePath.c_str();
        qDebug()<<oc.text();
    }
}
