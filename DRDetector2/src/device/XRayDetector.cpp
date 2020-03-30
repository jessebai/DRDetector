#include "XRayDetector.h"

XRayDetector::XRayDetector(QObject *parent): QObject(parent)
{

}

int XRayDetector::registerCallBack(){
    m_pCallBack = new DetectorEventHandler();
    return CR_RegisterEventCallback(m_nDeivceId, m_pCallBack);
}

void XRayDetector::initDevice(){
    std::thread initThread(&XRayDetector::initProgress, this);
    initThread.detach();
}


//1.connect detector
//2.connect event handler
//3.retrieve mode from detector
//4.emit conneted signal
//5.emit mode update signal
void XRayDetector::initProgress(){
    int nRet = CR_Connect(m_nDeivceId);
    if(nRet < D_ERROR){
        emit deviceConnected();
        CR_ResetDetector(m_nDeivceId, false);
        CR_GetSystemInformation(m_nDeivceId, &m_sysInfo);
        qDebug()<<m_sysInfo.szDetrDesc;
        qDebug()<<m_sysInfo.szDetrMachineID;
        qDebug()<<m_sysInfo.szFirmwareVersion;
        qDebug()<<m_sysInfo.szHardwareVersion;
        qDebug()<<m_sysInfo.szSerialNumber;
        qDebug()<<m_sysInfo.szSoftwareVersion;

        m_pCallBack = new DetectorEventHandler();
        QObject::connect(m_pCallBack,SIGNAL(newFrameIdx(int)),this,SLOT(newFrameUpdate(int)));
        CR_RegisterEventCallback(m_nDeivceId, m_pCallBack);
        nRet = CR_GetApplicationMode(m_nDeivceId, m_modeInfos, &m_nModeCount);
        if(nRet < D_ERROR){
            emit modeUpdated();
        }
    }else{
        CR_GetLastIntlMsg(m_szMsgBuf, MAX_BUF_LEN);
        emit connectFailed(m_szMsgBuf);
    }
}



//get pre installed application modes in the device
int XRayDetector::getDeviceModes(){
    return CR_GetApplicationMode(m_nDeivceId, m_modeInfos, &m_nModeCount);
}

int XRayDetector::setModeWithParam(int curIdx, float framerate, int trigType, int gainLevel){
    int expTime = 0;        //default exposure time, no effect
    CR_ResetDetector(m_nDeivceId, false);
    int nRet = CR_SetApplicationModeWithParam(
                                        m_nDeivceId
                                        , m_modeInfos[curIdx].nModeID
                                        , &framerate
                                        , &expTime
                                        , trigType
                                        , gainLevel
                                            );
    if(nRet < D_ERROR){
        m_activeMode = m_modeInfos[curIdx];
        qDebug()<<m_activeMode.nModeID;
        m_fFrmRate = framerate;
        m_nTrigType = trigType;
        m_nGainLevel = gainLevel;
        m_nImgWidth = m_activeMode.nImageWidth;
        m_nImgHeight = m_activeMode.nImageHeight;
        m_nFrmNum = 20;
        m_nImgSize = m_nImgWidth * m_nImgHeight;
        m_nFrmSize = m_nImgSize + FRAME_HEADER_LEN/2;
        initFrameBuffer();
    }
    qDebug()<<m_nImgWidth<<m_nImgHeight;
    return nRet;
}

QString XRayDetector::getActiveModeInfo(){
    qDebug()<<m_activeMode.nModeID;
    QString mode_info(m_activeMode.szDesc);

    return mode_info;
}

void XRayDetector::initFrameBuffer(){
    if(m_pFrmBuf!=nullptr){
        free(m_pFrmBuf);
        m_pFrmBuf = nullptr;
    }
    m_pFrmBuf = reinterpret_cast<unsigned short*>(
                    malloc(
                         static_cast<uint>(m_nFrmSize * m_nFrmNum)
                        * sizeof(unsigned short)
                    )
                );
    qDebug()<<"frame num"<<m_nFrmNum;
    qDebug()<<"buffer size"<<m_nFrmSize*m_nFrmNum*2<<"bytes";
    qDebug()<<"frame size"<<m_nFrmSize*2<<"bytes";
    m_pRetFrm = reinterpret_cast<unsigned short*>(
                            malloc(static_cast<uint>(m_nImgSize)
                           * sizeof(unsigned short))
                );
    qDebug()<<"image size"<<m_nImgSize*2<<"bytes";
}

int XRayDetector::darkCalibration(){
    int nRet = CR_StartDarkCalibration(m_nDeivceId);
    if(nRet < D_ERROR){
        qDebug()<<"new thread";
        std::thread calibThread(&XRayDetector::calibrationProgress, this);
        calibThread.detach();
    }
    qDebug()<<"start"<<nRet;
    return nRet;
}

int XRayDetector::gainCalibration(){
    int nRet = CR_StartGainCalibration(m_nDeivceId);
    if(nRet < D_ERROR){
        std::thread calibThread(&XRayDetector::calibrationProgress, this);
        calibThread.detach();
    }
    return nRet;
}

void XRayDetector::calibrationProgress(){
    CR_CalibrationInfo calProgInfo;
    int nCurFrmNum = 0;
    do{
        CR_QueryCalibrationStatus(m_nDeivceId, &calProgInfo);
        if(calProgInfo.nCurrentFrameNum!=nCurFrmNum){
            qDebug()<<"calibrating";
            nCurFrmNum = calProgInfo.nCurrentFrameNum;
            emit calibProgressing(nCurFrmNum, calProgInfo.nTotalFrameNum);
        }

    }while(-1 == calProgInfo.nResult);
    emit calibrationFinish();
}

int XRayDetector::startAquisition(){
    m_nCurAcc = 0;
    int nRet =  CR_StartAcquisition(m_nDeivceId, m_nFrmNum, m_pFrmBuf);
    qDebug()<<"start success?:"<<nRet;
    return nRet;
}

int XRayDetector::stopAquisition(){
    int nRet = CR_StopAcquisition(m_nDeivceId);
    qDebug()<<"stop aquisition"<<nRet;
    return nRet;
}

QString XRayDetector::getErrMsg(){
    CR_GetLastIntlMsg(m_szMsgBuf, MAX_BUF_LEN);
    return m_szMsgBuf;
}

void XRayDetector::newFrameUpdate(int nBuffIdx){
    qDebug()<<"device got raw frame"<<nBuffIdx;
    unsigned short* pFrame = m_pFrmBuf + nBuffIdx * m_nFrmSize + FRAME_HEADER_LEN/2;
    //unsigned short* pStart = m_pFrmBuf + nBuffIdx * m_nFrmSize;
    int convertedIdx = 0;
    for(int i = m_nImgWidth - 1; i >= 0; i--){
        for (int j = 0; j < m_nImgHeight; j++){
            int idx = j*m_nImgWidth + i;
            int pixSum = static_cast<int>(m_pRetFrm[convertedIdx] * m_nCurAcc + pFrame[idx]);
            m_pRetFrm[convertedIdx] = static_cast<unsigned short>(pixSum/(m_nCurAcc + 1));
            convertedIdx++;
        }
    }
    m_nCurAcc++;

    if(m_nCurAcc >= m_nAccFrmCnt){
        m_nCurAcc = 0;
        qDebug()<<"single result frame";
        DcmProcess::saveAsDcmFile(m_pRetFrm, m_nImgHeight, m_nImgWidth, m_sFilePath);
    }
    //stopAquisition();
    //emit newFrame(reinterpret_cast<long>(m_pRetFrm), m_nImgHeight, m_nImgWidth);

}

bool XRayDetector::getIsConnected() const
{
    return isConnected;
}

QString XRayDetector::getModeId(int index){
    return QString(m_modeInfos[index].nModeID);
}

QString XRayDetector::getModeDesc(int index){
    return QString(m_modeInfos[index].szDesc);
}

QString XRayDetector::getMaxFrmRate(int index){
    return QString::number(static_cast<double>(m_modeInfos[index].fMaxFrmRate));
}

QString XRayDetector::getMaxExpTime(int index){
    return QString::number(static_cast<double>(m_modeInfos[index].nMaxExpTime));
}

int XRayDetector::getDefaultTrigType(int index){
    return m_modeInfos[index].nDefaultTrigType;
}

int XRayDetector::getDefaultGainLevel(int index){
    return m_modeInfos[index].nDefaultGainLevel;
}

QJsonArray XRayDetector::getTrigTypes(int index){
    QJsonArray trigTypes;
    CR_ModeInfo selected = m_modeInfos[index];
    for(int i = 0; i < selected.nTrigTypeNum;i++){
        trigTypes.append(selected.nTrigTypes[i]);
    }
    return trigTypes;
}

QJsonArray XRayDetector::getGainlevels(int index){
    QJsonArray gainLevels;
    CR_ModeInfo selected = m_modeInfos[index];
    for(int i = 0; i < selected.nGainLevelNum;i++){
        gainLevels.append(selected.nGainLevels[i]);
    }
    return gainLevels;
}

void XRayDetector::setFilePath(QString str){
    m_sFilePath = str;
}

QString XRayDetector::getFilePath(){
    return m_sFilePath;
}

void XRayDetector::test(){
    qDebug() << m_sFilePath.toStdString().c_str();


    time_t rawtime;
    time(&rawtime);
    char cc[32];
    _itoa_s(rawtime, cc, 10);
    strcat_s(cc, ".dcm");
    qDebug()<<"savedfile"<< string(cc).c_str();
    //char filename[32];
    //std::strftime(filename, 32, "%a, %Y-%m-%d-%H-%M-%S", rawtime);


    m_sFilePath = m_sFilePath + "\\" + QString(cc);
    qDebug() << m_sFilePath;
    qDebug() << m_sFilePath.toStdString().c_str();
}
