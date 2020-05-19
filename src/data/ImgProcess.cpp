#include "ImgProcess.h"

ImgProcess::ImgProcess()
{

}

Mat ImgProcess::buffToMat(unsigned short* pFrame, int nWidth, int nHeight){
    Mat imageOrigGray = Mat(nHeight, nWidth, CV_16U, pFrame);
    return imageOrigGray;
}



vector<int> ImgProcess::stretchlim(const Mat& src, const double p1, const double p2)
{
    vector<int> res(2, 2);
    int area = src.cols*src.rows;
    int k1 = int(p1 * area);
    int k2 = int(p2 * area);
    if (k1 == 0)
        k1 = 1;
    Mat temp = src,tempSort;
    temp=temp.reshape(0, 1);
    cv::sort(temp, tempSort, SORT_ASCENDING);
    res[0] = tempSort.at<ushort>(0, k1);
    res[1] = tempSort.at<ushort>(0, k2);
    return res;
}

Mat ImgProcess::createSamples(Mat & stc, vector<int>& vnCoordnt, int nLargestH, int nLargestW)
{
    //0 - left, 1 - top
    return stc(Range(vnCoordnt[1] - 5, vnCoordnt[1] + nLargestH + 5),
        Range(vnCoordnt[0] - 5, vnCoordnt[0] + nLargestW + 5));
}


Mat ImgProcess::gammaTransform(const Mat & src, const vector<int>inLim, float gamma)
{
    Mat mUpper = src >= inLim[1];
    Mat mMiddle = (src>inLim[0]) & (src<inLim[1]);
    mMiddle /= 255;
    mMiddle.convertTo(mMiddle, CV_32F);
    mUpper.convertTo(mUpper, CV_32F);
    mUpper *= 255;
    Mat dst;
    src.convertTo(dst,CV_32F);

    pow((dst - inLim[0])*65535.0 / (inLim[1] - inLim[0]),gamma,dst);
    dst = dst.mul(mMiddle);
    dst += mUpper;
    return dst;
}

void ImgProcess::fillHole(const Mat srcBw, Mat &dstBw)
{
    Size m_Size = srcBw.size();
    Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());
    srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

    floodFill(Temp, Point(0, 0), Scalar(255));

    Mat cutImg;
    Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

    dstBw = srcBw | (~cutImg);
}

void ImgProcess::mainProcess(Mat gOrg, vector<Mat>& res,	vector<bool>& isconform, vector<vector<Point>>& vvPDefCoordinates)
{
    Mat gOrg8, bOrg;
    gOrg.convertTo(gOrg8, CV_8U, 1 / 255.0);
    threshold(gOrg8, bOrg, 0, 255, THRESH_BINARY_INV|THRESH_OTSU);

    Mat bOrgLabel, mOrgStats, mOrgCentroids;
    connectedComponentsWithStats(bOrg, bOrgLabel, mOrgStats, mOrgCentroids);
    vector<vector<int>> vvnCoordnt;
    for (int i = 2; i < mOrgStats.rows; i++)
    {
        //(i, 4) total area(in pixels) of the connected components
        if (mOrgStats.at<int>(i, 4) > 70000)
        {

            vector<int> vnTemp;
            for (int j = 0; j < 4; j++)
            {
                vnTemp.push_back(mOrgStats.at<int>(i, j));
            }
            vvnCoordnt.push_back(vnTemp);
        }
    }
    int nNumBot = vvnCoordnt.size(), nLargestH = 0, nLargestW = 0;
    for (int i = 0; i < nNumBot; i++)
    {
        //height of area
        nLargestH = max(nLargestH, vvnCoordnt[i][3]);
        //width of area
        nLargestW = max(nLargestW, vvnCoordnt[i][2]);
    }

    Mat se = getStructuringElement(MORPH_RECT, Size(7, 7));
    Mat gFilter;
    morphologyEx(gOrg, gFilter, MORPH_BLACKHAT, se);

    Mat gFilterMedian;
    medianBlur(gFilter, gFilterMedian, 3);

    Mat gSglFilter,gSgl;
    for (int idxBot = 0; idxBot < nNumBot; idxBot++)
    {

        gSgl=createSamples(gOrg, vvnCoordnt[idxBot], nLargestH, nLargestW);

        Mat gSglMed;
        medianBlur(gSgl, gSglMed, 3);

        int uCapLine = 135;
        Mat gCap, gCapAdj;
        gCap = gSglMed(Range(0, uCapLine + 1), Range::all());

        gCapAdj = gammaTransform(gCap,
            stretchlim(gCap, 0.01, 0.6),
            1);
        gCapAdj.convertTo(gCapAdj, CV_16UC1);

        Mat gBot, gBotAdj;
        gBot = gSglMed(Range(uCapLine + 1, gSglMed.rows), Range::all());
        gBotAdj = gammaTransform(gBot,
            stretchlim(gBot, 0.07, 0.38),
            1);
        gBotAdj.convertTo(gBotAdj, CV_16UC1);

        Mat bGasInOut = gBotAdj > 63333;

        Mat bGasInOut8, bGasInOutLabel, bGas;
        bGasInOut.convertTo(bGasInOut8, CV_8U);
        connectedComponents(bGasInOut8, bGasInOutLabel);
        bGas = bGasInOutLabel == 2;

        Mat vAvg;
        reduce(bGas,vAvg,1,REDUCE_AVG);

        int uLineDown;
        for (uLineDown = bGas.rows - 1; uLineDown >= 0; uLineDown--)
        {
            if (vAvg.at<uchar>(uLineDown) >= 0.6 * 255)
                break;
        }

        gSglFilter = createSamples(gFilterMedian, vvnCoordnt[idxBot], nLargestH, nLargestW);

        Mat gSglFilterMed;
        medianBlur(gSglFilter, gSglFilterMed, 3);


        Mat gSglFilterAdj;
        gSglFilterAdj = gammaTransform(gSglFilterMed,
            stretchlim(gSglFilterMed, 0.85, 0.93),
            1);
        gSglFilterAdj.convertTo(gSglFilterAdj, CV_8U,1/255.0);

        Mat bSglFilter;
        threshold(gSglFilterAdj, bSglFilter, 0, 255, THRESH_BINARY|THRESH_OTSU);

        bSglFilter.row(uCapLine + 5).setTo(255);

        Mat bSglFilterFill, bSglFilterInside, bSglFilterInsideComplement;
        fillHole(bSglFilter, bSglFilterFill);

        bSglFilterInside = bSglFilterFill - bSglFilter;
        bSglFilterInsideComplement = ~bSglFilterInside;

        Mat bDefLabel, bDef;
        connectedComponents(bSglFilterInsideComplement, bDefLabel);

        bDef = bDefLabel >= 2;

        Mat bSglFilterInsideComplementOpen, bDefAdd, bDefTotal;
        morphologyEx(bSglFilterInsideComplement, bSglFilterInsideComplementOpen,
            MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(7, 7)));
        bDefAdd = bSglFilterInsideComplement - bSglFilterInsideComplementOpen;

        bDefTotal = bDefAdd + bDef;
        bDefTotal.rowRange(0, uCapLine + 10).setTo(0);
        bDefTotal.rowRange(vvnCoordnt[idxBot][3] + 5, bDefTotal.rows).setTo(0);

        vector<Point> vPDefSgl;
        findNonZero(bDefTotal, vPDefSgl);
        //relative coordinates to absolute coordinates
        for (int i = 0; i < vPDefSgl.size(); i++) {
            vPDefSgl[i].x += vvnCoordnt[idxBot][0];
            vPDefSgl[i].y += vvnCoordnt[idxBot][1];
        }
        vvPDefCoordinates.push_back(vPDefSgl);

        int uLineBottom = 322;
        gBotAdj.row(uLineBottom).setTo(255);
        gBotAdj.row(uLineDown).setTo(255);
        Mat gBotDown, gBotUp, gBotUpAdj;
        gBotDown = gBotAdj(Range(uLineDown + 1, gBotAdj.rows), Range::all());
        gBotDown.convertTo(gBotDown, CV_16UC1);
        gBotUp = gBot(Range(0, uLineDown + 1), Range::all());
        gBotUpAdj = gammaTransform(gBotUp,
            stretchlim(gBotUp, 0.1, 0.6),
            1);
        gBotUpAdj.convertTo(gBotUpAdj, CV_16UC1);

        Mat gTotal;
        vconcat(gCapAdj, gBotUpAdj, gTotal);
        vconcat(gTotal, gBotDown, gTotal);

        Mat gDef;
        gDef = ~bDefTotal;
        gDef.convertTo(gDef, CV_16UC1);
        //cout << gDef;
        Mat gBotNoDef = gTotal.mul(gDef / 255);
        gBotNoDef.convertTo(gBotNoDef, CV_8U,1/255.0);

        res.push_back(gBotNoDef);


        bDefTotal.convertTo(bDefTotal, CV_32F);
        Mat bDefCol(bDefTotal.rows, 1, CV_32F), bDefPixels(1, 1, CV_32F);

        reduce(bDefTotal, bDefCol, 1, REDUCE_SUM);
        reduce(bDefCol, bDefPixels, 0, REDUCE_SUM);

        if (bDefPixels.at<float>(0, 0) >= 300)
            isconform.push_back(true);
        else
            isconform.push_back(false);
    }
    return;
}
