#ifndef XRAYDETECTOR_H
#define XRAYDETECTOR_H

#include <QObject>
#include <QJsonArray>
#include <QtDebug>
#include <QPointer>

#include <thread>

#include "CareRayAPIDll.h"
#include "src/data/DcmProcess.h"
#include "IntlMsgAliasID.h"


#include "DetectorEventHandler.h"
#define D_ERROR 300000
#define MSG_BUF_SIZE 256

class XRayDetector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int modeCount READ modeCount)
    Q_PROPERTY(bool conneted READ getIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(int activeModeIdx READ activeModeIdx)
public:
    //constructor
    XRayDetector(QObject *parent = nullptr);

    //device callback register
    Q_INVOKABLE int registerCallBack();
    //device initialize
    Q_INVOKABLE void initDevice();

    //connect device asynchronously
    void initProgress();


    // application modes
    Q_INVOKABLE int getDeviceModes();
    Q_INVOKABLE int setModeWithParam(int curIdx, float framerate, int trigType, int gainType);
    Q_INVOKABLE QString getActiveModeInfo();
    void initFrameBuffer();


    //calibration
    Q_INVOKABLE int darkCalibration();
    Q_INVOKABLE int gainCalibration();
    void calibrationProgress();

    //device manipulation
    Q_INVOKABLE int startAquisition();
    Q_INVOKABLE int stopAquisition();

    Q_INVOKABLE QString getErrMsg();

    //mode info retrival
    Q_INVOKABLE QString getModeId(int idx);
    Q_INVOKABLE QString getModeDesc(int idx);
    Q_INVOKABLE QString getMaxFrmRate(int idx);
    Q_INVOKABLE QString getMaxExpTime(int idx);
    Q_INVOKABLE int getDefaultTrigType(int idx);
    Q_INVOKABLE int getDefaultGainLevel(int idx);
    Q_INVOKABLE QJsonArray getTrigTypes(int idx);
    Q_INVOKABLE QJsonArray getGainlevels(int idx);


    //attribute getter setter
    DetectorEventHandler* callBack(){ return m_pCallBack;}
    int modeCount(){ return m_nModeCount;}
    int activeModeIdx(){ return m_nActiveModeIdx;}


    //getter and setter
    bool getIsConnected() const;

    //自动保存路径和名称
    Q_INVOKABLE void setFilePath(QString str);
    Q_INVOKABLE QString getFilePath();

    Q_INVOKABLE void test();


signals:
    void isConnectedChanged();
    void deviceConnected();
    void connectFailed(QString error);
    void modeUpdated();
    void calibProgressing(int nCurFrmNum, int nTotalFrmNum);
    void calibrationFinish();
    void newFrame(long frmAddr, int nWidth, int nHeight);

public slots:
    void newFrameUpdate(int frameIdx);
private:
    int m_nDeivceId = 1;

    int m_nAccFrmCnt = 1; //number of frames to calcaulate into a single frame
    int m_nCurAcc = 0;

    //call back handler
    DetectorEventHandler* m_pCallBack;


    //device parameter
    int m_nActiveModeIdx;
    int m_nModeId;
    int m_nModeCount;
    CR_ModeInfo m_activeMode;
    CR_ModeInfo m_modeInfos[16];

    //device status
    bool isConnected = false;
    CR_SystemInfo m_sysInfo;

    //device response message
    const int MAX_BUF_LEN = MSG_BUF_SIZE;
    char m_szMsgBuf[MSG_BUF_SIZE];

    //active device config
    float m_fFrmRate;
    int m_nTrigType;
    int m_nGainLevel;

    //device data buffer
    int m_nFrmNum = 20;          //number of frames in buffer
    unsigned short* m_pFrmBuf = nullptr;          //pointer to buffer
    unsigned short* m_pRetFrm = nullptr;          //a resulting single frame from multiple raw frames

    //image parameter
    int m_nImgHeight;
    int m_nImgWidth;
    int m_nImgSize;
    int m_nFrmSize;


    QString m_sFilePath;

};

#endif // XRAYDETECTOR_H
