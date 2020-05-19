#include "DcmImageView.h"

DcmImageView::DcmImageView(QQuickItem  *parent) : QQuickPaintedItem(parent)
{

}

void DcmImageView::paint(QPainter *painter)
{
    QImage scaled;
    qDebug()<<"final paint para"<<m_dZoom<<disWidth()<<disHeight();
    scaled = m_activeImage.scaled(
                disWidth(),
                disHeight(),
                Qt::KeepAspectRatioByExpanding
                );
    painter->drawImage(QPointF(0,0), scaled);
}

void DcmImageView::freeData(void* data){
    free(data);
}

void DcmImageView::updateImage(){
    m_activeImage = generateQImage(m_nColorLow, m_nColorHigh);
    qDebug()<<"active image"<<m_activeImage.width();
}

void DcmImageView::drawImage(){
    updateImage();
    qDebug()<<"ready to paint";
    update();
}

QImage DcmImageView::generateQImage(int colorLow, int colorHigh){
    qDebug()<<"generating qImage";
    if(m_bAddInfo){
        qDebug() << "addStr in generateQImage()";
        m_curImgData->addStr(m_sInfo, m_nPositionX, m_nPositionY, m_nFontSize, m_cFontType);
        m_bAddInfo = false;
    }
    int windowSize = colorHigh - colorLow;
    unsigned short* pRawImage = m_curImgData->pRawImage();

    // uchar* pConvertFrame = reinterpret_cast<uchar*>(malloc(m_nRawImgHeight * m_nRawImgWidth * sizeof(uchar)));
    //QImage m_activeQImage(m_nRawImgWidth, m_nRawImgHeight, QImage::Format_RGB888);
    unsigned short color;
    uchar* imageData = m_activeQImage.bits();

    for(int i = 0; i < m_nRawImgHeight; i++){
        for(int j = 0; j < m_nRawImgWidth; j++){
            int nPixIdx = i * m_nRawImgWidth + j;
            unsigned short nOriColor = pRawImage[nPixIdx];
            if(nOriColor > colorHigh){
                color = 255;
            }else if(nOriColor < colorLow){
                color = 0;
            }else{
                color = static_cast<uchar>(
                            255
                            * static_cast<double>((nOriColor - colorLow))
                            / windowSize
                            );
            }
            imageData[nPixIdx*3] = color;
            imageData[nPixIdx*3 + 1] = color;
            imageData[nPixIdx*3 + 2] = color;
            //m_activeQImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
    return m_activeQImage;
}

void DcmImageView::loadCurImage(ImageData* curImg){
    m_curImgData = curImg;
    m_nRawImgHeight = m_curImgData->nRawImgHeight();
    m_nRawImgWidth = m_curImgData->nRawImgWidth();
    m_activeQImage = QImage(m_nRawImgWidth, m_nRawImgHeight, QImage::Format_RGB888);
    m_pRawImage = m_curImgData->pRawImage();
    qDebug()<<"image loaded"<<m_nRawImgHeight<<m_nRawImgWidth;
}

void DcmImageView::saveCurImage(QString filePath, int colorLow, int colorHigh){
    int len = filePath.size();
    filePath = filePath.right(len-8);
    m_nRawImgHeight = m_curImgData->nRawImgHeight();
    m_nRawImgWidth = m_curImgData->nRawImgWidth();
    unsigned short *curImg = m_curImgData->pRawImage();
    if(filePath.right(3) != "dcm"){
        m_activeQImage = generateQImage(colorLow, colorHigh);
        qDebug() << filePath.toStdString().data();
        m_activeQImage.save(filePath);
//        bool status = imwrite(filePath.toStdString(), curImgMat);
//        if(status)
//            qDebug() << "save succeed";
//        else
//            qDebug() << "save failed";
    } else {
//        qDebug() << filePath;
//        len = filePath.size();
//        int pos = 0;
//        for(int i = len - 1; i >= 0; i--){
//            if(filePath[i] == '/'){
//                pos = i;
//                break;
//            }
//        }
//        QString fileName = filePath.right(len - 1 - pos);
        DcmProcess::saveAsDcmFileWithPath(curImg, m_nRawImgWidth, m_nRawImgHeight, filePath.toStdString());
    }
}

//determine initial zoom value according to image size
void DcmImageView::initZoom(){
    m_displayArea = this->boundingRect();
    m_dZoom = 1.0 * m_displayArea.width() / m_nRawImgWidth;
    qDebug()<<m_dZoom;
}

void DcmImageView::zoomIn(){
    m_dZoom *= 1.25;
    if(m_dZoom > pow(1.25, 5))
        m_dZoom = pow(1.25, 5);
    qDebug() << m_dZoom;
}

void DcmImageView::zoomOut(){
    m_dZoom /= 1.25;
}

int DcmImageView::posToCoor(double pos){
    return static_cast<int>(pos / m_dZoom);
}

int DcmImageView::getPixelValue(double x, double y){
    if(m_activeImage.isNull())
        return 0;
    int real_x = static_cast<int>(x / m_dZoom);
    int real_y = static_cast<int>(y / m_dZoom);
    return m_pRawImage[real_y*m_nRawImgWidth + real_x];
}

void DcmImageView::changeColorWindow(double x1, double y1, double x2, double y2){
    int real_left = posToCoor(x1 <= x2 ? x1 : x2);
    int real_right = posToCoor(x1 <= x2 ? x2 : x1);
    int real_top = posToCoor(y1 <= y2 ? y1 : y2);
    int real_buttom = posToCoor(y1 <= y2 ? y2 : y1);
    int minColor = 65535;
    int maxColor = 0;

    for(int i = real_top; i < real_buttom; i++){
        for(int j = real_left; j < real_right; j++){
            int color = m_pRawImage[i * m_nRawImgWidth + j];
            minColor = std::min(minColor,color);
            maxColor = std::max(maxColor, color);
        }
    }
    m_nColorLow = minColor;
    m_nColorHigh = maxColor;
    emit colorWindowChanged();
    qDebug()<<"new color window"<<minColor<<maxColor;
}

void DcmImageView::setLow(int newLow) {
    m_nColorLow = newLow;
    if(!m_activeImage.isNull()){
        updateImage();
        update();
    }
}

void DcmImageView::setHigh(int newHigh) {
    m_nColorHigh = newHigh;
    if(!m_activeImage.isNull()){
        updateImage();
        update();
    }
}


Mat DcmImageView::p2mat(void* data){
    return Mat(m_nRawImgHeight,m_nRawImgWidth,CV_16UC1,data);
}

void DcmImageView::setImgInfo(QString addInfo, QString str, QString positionX, QString positionY, QString fontSize, QString fontType){
    if(addInfo == "显示水印")
        m_bAddInfo = true;
    else
        m_bAddInfo = false;
    m_sInfo = str;
    m_nPositionX = positionX.toInt();
    m_nPositionY = positionY.toInt();
    m_nFontSize = fontSize.toInt();
    m_cFontType = fontType.toStdString().data();
    if(m_bAddInfo && m_pRawImage != nullptr){
        m_activeImage = generateQImage(m_nColorLow, m_nColorHigh);
        update();
    }
}

void DcmImageView::clearImgInfo(){
    m_curImgData->copyImage();
    m_bAddInfo = false;
    m_activeImage = generateQImage(m_nColorLow, m_nColorHigh);
//    qDebug() << m_activeImage[137][276];
    update();
}

void DcmImageView::setMeasureScale(double scale){
    m_dMeasureScale = scale;
}

double DcmImageView::getMeasureScale(){
    return m_dMeasureScale;
}

void DcmImageView::countMeasureScale(double x1, double x2, double y1, double y2){
    double width = abs(x2 - x1);
    double height = abs(y2 - y1);
    double len = sqrt(pow(width, 2) + pow(height, 2));
    m_dMeasureScale = m_dFixedLength / len;
    qDebug() << x1 << x2 << y1 << y2 << len << " " << m_dMeasureScale;
}

QString DcmImageView::countLength(double x1, double x2, double y1, double y2){
    double width = abs(x2 - x1);
    double height = abs(y2 - y1);
    double pixLen = sqrt(pow(width, 2) + pow(height, 2));
    double actualLen = pixLen * m_dMeasureScale;
    QString str = QString::number(actualLen, 'f', 2);
    qDebug() << x1 << x2 << y1 << y2 << pixLen << " " << actualLen;
    return str;
}

QString DcmImageView::countAngle(double x1, double x2, double y1, double y2){
    double width = abs(x2 - x1);
    double height = abs(y2 - y1);
    double angle = atan2(height, width)/3.1415926*180;
    QString str = QString::number(angle, 'f', 2);
    qDebug() << angle;
    return str;
}

QString DcmImageView::countArea(double x1, double x2, double y1, double y2){
    double width = abs(x2 - x1);
    double height = abs(y2 - y1);
    double area = width * height * m_dMeasureScale * m_dMeasureScale;
    QString str = QString::number(area, 'f', 2);
    return str;
}

double DcmImageView::getZoom(){
    return m_dZoom;
}

QString DcmImageView::countAve(double x1, double x2, double y1, double y2){
    double width = abs(x2 - x1) * m_dMeasureScale;
    double height = abs(y2 - y1) * m_dMeasureScale;
    double area = width * height;
    double temp = 0;
    if(x1 > x2){
        temp = x2;
        x2 = x1;
        x1 = temp;
    }
    if(y1 > y2){
        temp = y2;
        y2 = y1;
        y1 = temp;
    }
    int X1 = int(x1);
    int X2 = int(x2);
    int Y1 = int(y1);
    int Y2 = int(y2);

    qDebug() << "X1: " << X1 << "X2: " << X2 << "Y1: " << Y1 << "Y2: " << Y2;

    unsigned short* pRawImage = static_cast<unsigned short*>(m_curImgData->pRawImage());

    //计算均值1
    double sum = 0;
    for(int i = Y1; i <= Y2; i++){
        for(int j = X1; j <= X2; j++){
            int nPixIdx = i * m_nRawImgWidth + j;
            unsigned short nOriColor = static_cast<unsigned short>(pRawImage[nPixIdx]);
            sum = static_cast<double>(nOriColor) + static_cast<double>(sum);
//            //m_activeQImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
//    double ave = (double)sum / (double)((X2-X1+1) * (Y2-Y1+1));
    double ave = 0;

    QString str = QString::number(ave, 'f', 2);
    return str;
}

QString DcmImageView::countSigma(double ave, double x1, double x2, double y1, double y2){
    unsigned short* pRawImage = m_curImgData->pRawImage();
    double temp = 0;
    if(x1 > x2){
        temp = x2;
        x2 = x1;
        x1 = temp;
    }
    if(y1 > y2){
        temp = y2;
        y2 = y1;
        y1 = temp;
    }
    int X1 = int(x1);
    int X2 = int(x2);
    int Y1 = int(y1);
    int Y2 = int(y2);
    //计算方差
    double sum = 0;
    for(int i = Y1; i <= Y2; i++){
        for(int j = X1; j <= X2; j++){
            int nPixIdx = i * m_nRawImgWidth + j;
            unsigned short nOriColor = pRawImage[nPixIdx];
            sum += (nOriColor - ave) * (nOriColor - ave);
            //m_activeQImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
    double sigmaSquare = (double)sum/(double)((X2-X1+1) * (Y2-Y1+1));
    double sigma = sqrt(sigmaSquare);
    QString str = QString::number(sigma, 'f', 2);
    return str;
}

QString DcmImageView::countSNR(double ave, double sigma){
    double snr = ave / sigma;
    qDebug() << "ave: " << ave << "sigma: " << sigma << "snr: " << snr;
    QString str = QString::number(snr, 'f', 2);
    return str;
}

//QVector<int> DcmImageView::countDwArray(double x1, double x2, double y1, double y2){
//    QVector<int> res;
//    return  res;
//}

//QVector<double> DcmImageView::countDw(double x1, double x2, double y1, double y2){
//    QVector<double> res;
//    return  res;
//}

QVector<int> DcmImageView::countDwArray(double x1, double x2, double y1, double y2){
    qDebug() << "x1: " << x1 << "x2: " << x2 << "y1: " << y1 << "y2: " << y2;
    double temp = 0;
    if(x1 > x2){
        temp = x2;
        x2 = x1;
        x1 = temp;
    }
    if(y1 > y2){
        temp = y2;
        y2 = y1;
        y1 = temp;
    }
    QVector<int> intensity;
    double k = (y2 - y1) / (x2 - x1);
    double b = y1 - k * x1;
    int x = x1;
    int y = y1;
    while(x <= x2 || y <= y2){
        unsigned short* pRawImage = m_curImgData->pRawImage();
        int nPixIdx = y * m_nRawImgWidth + x;
        unsigned short nOriColor = pRawImage[nPixIdx];
        intensity.push_back(nOriColor);
        if(k * x + b - y <=0){ //under the line
            x++;
        } else {
            y++;
        }
    }
//    for(int i = 0; i < intensity.size(); i++){
//        qDebug() << "number" << i << ":" << intensity[i];
//    }

    return intensity;
}

QVector<double> DcmImageView::countDw(double x1, double x2, double y1, double y2){
    int topPercentage = 15;//top15 pixels as the start point
    int lineNum = 13;//lines in the picture(double lines still count num 1)
    //极小值判定范围0.1*len/13 1
    double lowPixelsRangePercentage = 0.2;
    //极小值与范围内最大值的差值，与整个曲线最小最大值的差值，的比例1
    double maxDifferencePercentage = 0.08;
    //判断两个极小值是否属于双线，小于该值是1
    double neighbourLowPixelsRangePercentage = 0.5;

    QVector<int> pixels = countDwArray(x1, x2, y1, y2);
    int len = pixels.length();
    int waveRange = len/lineNum;
    int topLen = len/topPercentage;

    QVector<int> pixelsSort = pixels;
    for(int i = 0; i < len-1; i++){
        for(int j = 0; j < len-1-i; j++){
            if(pixelsSort[j] < pixelsSort[j+1]){
                double temp = 0;
                temp = pixelsSort[j+1];
                pixelsSort[j+1] = pixelsSort[j];
                pixelsSort[j] = temp;
            }
        }
    }
    int topkLimit = pixelsSort[topLen];
//    int bottomkLimit = pixelsSort[len-topLen-1];
    int maxPixel = pixelsSort[0];
    int minPixel = pixelsSort[len-1];
    int minMaxRange = topkLimit - minPixel;
    qDebug() << "topk: " << topkLimit;
//    qDebug() << "bottomkLimit: " << bottomkLimit;
    qDebug() << "maxPixel: " << maxPixel;
    qDebug() << "minPixel: " << minPixel;

    int startPointIndex = 0;
    for(int i = 0; i < len; i++){
        if(pixels[i] < topkLimit){
            startPointIndex = i;
            break;
        }
    }
    qDebug() << "startPointIndex: " << startPointIndex;


    //寻找局部极小值，要求：1.该点在局部范围内极小。2.该点与局部最大值相差较大。3.与上一个极小值距离在范围内1
    QVector<int> lowPixels;
    QVector<int> lowPixelsIndex;
    int lowPixelsRange = lowPixelsRangePercentage * len / lineNum;
    int maxDifference = minMaxRange * maxDifferencePercentage;
    qDebug() << "lowPixelsRange: " << lowPixelsRange << "maxDifference: " << maxDifference;
    for(int i = 0; i < len-lowPixelsRange; i++){
        int curPixel = pixels[i+lowPixelsRange/2];
        //是否为最小值1
        bool flag = true;
        int localMax = -100000;
        for(int j = i; j < i+lowPixelsRange; j++){
            if(pixels[j] > localMax){
                localMax = pixels[j];
            }
            if(curPixel > pixels[j]){
                flag = false;
                break;
            }
        }
        if(flag){
            if(localMax - curPixel > maxDifference && topkLimit - curPixel > maxDifference && (i+lowPixelsRange/2 - lowPixelsIndex.last()) < 1.5*len/lineNum){
                lowPixels.push_back(pixels[i+lowPixelsRange/2]);
                lowPixelsIndex.push_back(i+lowPixelsRange/2);
            }
        }
    }

    for(int i = 0; i < lowPixels.size()-1; i++){
        qDebug() << "index: " << lowPixelsIndex[i] << "value: " << lowPixels[i];
    }

    QVector<int> lowPixelsDoubleWine;
    QVector<int> lowPixelsDoubleWineIndex;
    QVector<int> lowPixelsSingleWineIndex;
    int SingleWineStartIndex = lowPixels.size();
    int neighbourLowPixelsRange = neighbourLowPixelsRangePercentage * len/lineNum;
    for(int i = 0; i < lowPixels.size()-1; i++){
        if(lowPixelsIndex[i+1] - lowPixelsIndex[i] < neighbourLowPixelsRange){
            //两个点是双线的极小值1
            lowPixelsDoubleWineIndex.push_back(lowPixelsIndex[i]);
            lowPixelsDoubleWineIndex.push_back(lowPixelsIndex[i+1]);
            lowPixelsDoubleWine.push_back(lowPixels[i]);
            lowPixelsDoubleWine.push_back(lowPixels[i+1]);
            i = i + 1;
        } else {
            //如果不是双线的极小值，需要判断是不是出了异常点还是接下来都没有双线的极小值1
            //寻找该点（1.一定范围内2.值接近）的极小值（3.小范围内极小4.所有可能点里最小）1
            QVector<int> lowPixelsIndexLikely;
            for(int k = lowPixelsIndex[i] - neighbourLowPixelsRange/2; k < lowPixelsIndex[i] + neighbourLowPixelsRange/2; k++){
                bool flag = true;
                for(int j = k - 1; j < k + 1; j++){
                    if(pixels[j] < pixels[k]){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    if(abs(pixels[k] - lowPixels[i]) < maxDifference && abs(pixels[k] - lowPixels[i]) > 0 && abs(k-lowPixelsIndex[i]) > 1){
                        lowPixelsIndexLikely.push_back(k);
                    }
                }
            }
            int anotherLowPixelIndex = lowPixelsIndexLikely[0];
            if(lowPixelsIndexLikely.size()!=0){
                for(int k = 0; k < lowPixelsIndexLikely.size(); k++){
                    if(pixels[lowPixelsIndexLikely[k]] < pixels[anotherLowPixelIndex]){
                        anotherLowPixelIndex = lowPixelsIndexLikely[k];
                    }
                }
                if(!lowPixelsDoubleWineIndex.empty() && lowPixelsIndex[i]-lowPixelsDoubleWineIndex.last()>lowPixelsRange/3 && anotherLowPixelIndex-lowPixelsDoubleWineIndex.last()>lowPixelsRange/3){
                    if(lowPixelsIndex[i] < anotherLowPixelIndex){
                        lowPixelsDoubleWineIndex.push_back(lowPixelsIndex[i]);
                        lowPixelsDoubleWineIndex.push_back(anotherLowPixelIndex);
                        lowPixelsDoubleWine.push_back(lowPixels[i]);
                        lowPixelsDoubleWine.push_back(pixels[anotherLowPixelIndex]);
                    } else{
                        lowPixelsDoubleWineIndex.push_back(anotherLowPixelIndex);
                        lowPixelsDoubleWineIndex.push_back(lowPixelsIndex[i]);
                        lowPixelsDoubleWine.push_back(pixels[anotherLowPixelIndex]);
                        lowPixelsDoubleWine.push_back(lowPixels[i]);
                    }
                }
            } else{
                SingleWineStartIndex = i;
                break;
            }
        }
    }
    for(int i = SingleWineStartIndex; i < lowPixels.size(); i++){
        lowPixelsSingleWineIndex.push_back(lowPixelsIndex[i]);
    }

    for(int i = 0; i < lowPixelsDoubleWine.size(); i++){
        qDebug() << "doubleWineIndex: " << lowPixelsDoubleWineIndex[i] << "value: " << lowPixelsDoubleWine[i];
    }
    for(int i = 0; i < lowPixelsSingleWineIndex.size(); i++){
        qDebug() << "singleWineIndex: " << lowPixelsSingleWineIndex[i] << "value: " << pixels[lowPixelsSingleWineIndex[i]];
    }
    //测试发现基本双线可以找齐，单线不一定能找齐，所以单线坐标需要推理一下1

    //找到双线之间的最大值位置1
    QVector<int> highPixelsDoubleWine;
    QVector<int> highPixelsDoubleWineIndex;
    for(int i = 0; i < lowPixelsDoubleWineIndex.size(); i = i + 2){
        int maxValue = -100000;
        int maxValueIndex = 0;
        for(int j = lowPixelsDoubleWineIndex[i]; j < lowPixelsDoubleWineIndex[i+1]; j++){
            if(maxValue < pixels[j]){
                maxValue = pixels[j];
                maxValueIndex = j;
            }
        }
        highPixelsDoubleWine.push_back(maxValue);
        highPixelsDoubleWineIndex.push_back(maxValueIndex);
    }
    for(int i = 0; i < highPixelsDoubleWine.size(); i++){
        qDebug() << "number: " << i << "highPixelsDoubleWine: " << highPixelsDoubleWine[i];
    }

    //根据上述结果计算dip = (A+B-2C)/(A+B)1
    //同时给出各个dip文字的中心位置，双线位于极大值处，单线位于极小值处，没有线就算一个位置1
    QVector<double> dip;
    QVector<double> dipLocation;
    for(int i = 0; i < highPixelsDoubleWine.size(); i++){
        int j = i*2;
        double A = topkLimit - lowPixelsDoubleWine[j];
        double B = topkLimit - lowPixelsDoubleWine[j+1];
        double C = topkLimit - highPixelsDoubleWine[i];
        double value = 0;
        if(A <= 0 || B <= 0 || C <= 0){
            value = 0;
        } else {
            value = (A+B-2*C)/(A+B);
        }
        dip.push_back(value);
        dipLocation.push_back(highPixelsDoubleWineIndex[i]);
    }
    for(int i = 0; i < lowPixelsSingleWineIndex.size(); i++){
        dip.push_back(0);
        dipLocation.push_back(lowPixelsSingleWineIndex[i]);
    }
    while(dip.size() < lineNum){
        dip.push_back(0);
        dipLocation.push_back(dipLocation.last() + len/lineNum);
    }

    for(int i = 1; i < dip.size(); i++){
        if(dip[i-1] - dip[i] < -0.05){
            dip[i] = 0;
        }
    }

    for(int i = 0; i < dip.size(); i++){
        qDebug() << "dip: " << dip[i];
    }
    for(int i = 0; i < dipLocation.size(); i++){
        qDebug() << "dipLocation: " << dipLocation[i];
    }

    QVector<double> res;
    res = dip + dipLocation;
    return res;
}

