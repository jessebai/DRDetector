#ifndef IMGPROCESS_H
#define IMGPROCESS_H

#include <QtDebug>
#include <QObject>

#include "time.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

using namespace std;
using namespace cv;

class ImgProcess : public QObject
{
    Q_OBJECT
public:
    ImgProcess();

    Mat buffToMat(unsigned short* pFrame, int nWidth, int nHeight);

    static Mat createSamples(Mat& stc, vector<int>& vnCoordnt, int nLargestH, int nLargestW);

    static vector<int> stretchlim(const Mat& src, const double p1, const double p2);

    static Mat gammaTransform(const Mat& src, const vector<int>lim, float gamma);

    static void fillHole(const Mat srcBw, Mat & dstBw);

    static void saveAsDcmFile(unsigned short* pImage, int nHeight, int nWidth);

    static void mainProcess(Mat gOrg, vector<Mat>& res, vector<bool>& isconform, vector<vector<Point>>& vvPDefCoordinates);
};

#endif // IMGPROCESS_H
