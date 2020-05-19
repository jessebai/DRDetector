#ifndef MVSCAMERA_H
#define MVSCAMERA_H

#pragma once

#include <QObject>
#include <QJsonArray>
#include <QtDebug>
#include <QPointer>
#include <QString>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>

#include <thread>
#include <vector>

//#include <afxwin.h>

#include "MyCamera.h"

#define STATUS_ERROR            -1
#define IMAGE_NAME_LEN          64



class MVSCamera : public QObject
{
    Q_OBJECT

/*ch:控件对应变量 | en:Control corresponding variable*/
public:
    // 软触发check
    bool    m_bSoftWareTriggerCheck;
    // ch:曝光时间 | en:Exposure Time
    double  m_dExposureEdit;
    // ch:增益 | en:Gain
    double  m_dGainEdit;
    // ch:帧率 | en:Frame Rate
    double  m_dFrameRateEdit;

    /*ch:设备显示下拉框 | en:Device display drop-down box*/
    int      m_nDeviceCombo;

    static unsigned char*  m_pBufForDriver;            // ch:用于从驱动获取图像的缓存 | en:Buffer to get image from driver
    static unsigned int    m_nBufSizeForDriver;

    static int m_nWidth;
    static int m_nHeight;
    static QImage m_qImage;


/*ch:内部函数 | en:Built-in function*/
public:
//    Q_INVOKABLE void ShowErrorMsg(string csMessage, int nErrorNum);

    /*ch:初始化相机操作 | en:Initialization*/

    Q_INVOKABLE QString findDevice(void);                       //find device
    Q_INVOKABLE int openDevice(void);                    // ch:打开相机 | en:Open Device

    /*ch:设置、获取参数操作 | en:Set and get parameters operation*/
    Q_INVOKABLE int setTriggerMode(void);                // ch:设置触发模式 | en:Set Trigger Mode
    Q_INVOKABLE int getTriggerMode(void);
    Q_INVOKABLE int getExposureTime(void);               // ch:设置曝光时间 | en:Set Exposure Time
    Q_INVOKABLE int setExposureTime(void);
    Q_INVOKABLE int getGain(void);                       // ch:设置增益 | en:Set Gain
    Q_INVOKABLE int setGain(void);
    Q_INVOKABLE int getFrameRate(void);                  // ch:设置帧率 | en:Set Frame Rate
    Q_INVOKABLE int setFrameRate(void);
    Q_INVOKABLE int getTriggerSource(void);              // ch:设置触发源 | en:Set Trigger Source
    Q_INVOKABLE int setTriggerSource(void);

    /*ch:图片保存 | en:Save Image*/
    Q_INVOKABLE int saveImage(void);                     // ch:保存图片 | en:Save Image

    /*ch:关闭销毁操作 | en:Close and deatroy operation*/
    Q_INVOKABLE int closeDevice(void);                   // ch:关闭设备 | en:Close Device

    /*一些示意demo上的功能操作*/
    Q_INVOKABLE void continusMode(void);
    Q_INVOKABLE void triggerMode(void);
    Q_INVOKABLE void startGrabbing(void);
    Q_INVOKABLE void stopGrabbing(void);
    Q_INVOKABLE void softwareTrigger(void);
    Q_INVOKABLE void softwareOnce(void);
    Q_INVOKABLE void saveBmp(void);
    Q_INVOKABLE void getParameter(void);
    Q_INVOKABLE void setParameter(void);


    Q_INVOKABLE void setParameterQml(double expoTime, double gain, double frmRate);
    Q_INVOKABLE double getExposureTimeQml(void);
    Q_INVOKABLE double getGainQml(void);
    Q_INVOKABLE double getFrameRateTimeQml(void);

    //设置图片缓存
    Q_INVOKABLE static int getImageBuffer(void);


/*ch:自定义变量 | en:User Defined Variable*/
private:
    /*ch:状态 | en:Status*/
    bool  m_bOpenDevice;                        // ch:是否打开设备 | en:Whether to open device
    static bool  m_bStartGrabbing;                     // ch:是否开始抓图 | en:Whether to start grabbing
    int   m_nTriggerMode;                       // ch:触发模式 | en:Trigger Mode
    int   m_nTriggerSource;                     // ch:触发源 | en:Trigger Source
    MV_SAVE_IAMGE_TYPE   m_nSaveImageType;      // ch:保存图像格式 | en:Save Image Type

    /*ch:设备相关 | en:Device Related*/
    static CMyCamera*      m_pcMyCamera;               // ch:CMyCamera封装了常用接口 | en:CMyCamera packed commonly used interface
    //HWND  m_hwndDisplay;                        // ch:显示句柄 | en:Display Handle
    MV_CC_DEVICE_INFO_LIST m_stDevList;         // ch:设备信息列表结构体变量，用来存储设备列表
    //en:Device information list structure variable used to store device list

    unsigned char*  m_pBufForSaveImage;         // ch:用于保存图像的缓存 | en:Buffer to save image
    unsigned int    m_nBufSizeForSaveImage;



public:
    //constructor
    MVSCamera(QObject *parent = nullptr);

signals:


public slots:

};

#endif // XRAYDETECTOR_H
