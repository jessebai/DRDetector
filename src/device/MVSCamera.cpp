#include "stdafx.h"
#include "MVSCamera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int MVSCamera::m_nBufSizeForDriver = 0;
unsigned char* MVSCamera::m_pBufForDriver = nullptr;
int MVSCamera::m_nWidth = 0;
int MVSCamera::m_nHeight = 0;
bool MVSCamera::m_bStartGrabbing = false;
CMyCamera* MVSCamera::m_pcMyCamera = new CMyCamera();


MVSCamera::MVSCamera(QObject *parent): QObject(parent)
{

}

QString MVSCamera::findDevice(){
    qDebug() << "in the findDevice()"<<endl;
    QString strMsg = "";

    // ch:枚举子网内所有设备 | en:Enumerate all devices within subnet
    int nRet = CMyCamera::EnumDevices(&m_stDevList);
    if (MV_OK != nRet)
    {
        return strMsg;
    }

    // ch:将值加入到信息列表框中并显示出来 | en:Add value to the information list box and display
    unsigned int i;
    int nIp1, nIp2, nIp3, nIp4;
    for (i = 0; i < m_stDevList.nDeviceNum; i++)
    {
        MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
        if (nullptr == pDeviceInfo)
        {
            continue;
        }
        if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
        {
            nIp1 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
            nIp2 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
            nIp3 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
            nIp4 = (m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

            char* pUserName = reinterpret_cast<char*>(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName);

            strMsg = "GigE:" + QString(*pUserName);
//            strMsg.Format(_T("[%d]GigE:    %s  (%d.%d.%d.%d)"), i,
//                          pUserName, nIp1, nIp2, nIp3, nIp4);
        }
        else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
        {
            char* pUserName = reinterpret_cast<char*>(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);

            strMsg = "UsbV3:" + QString(*pUserName);
        }
        else
        {
            strMsg = "Unknown device enumerated";
//            ShowErrorMsg(TEXT("Unknown device enumerated"), 0);
        }


        //need to be completed, show the msg on the screen


    }

    if (0 == m_stDevList.nDeviceNum)
    {
        strMsg = "No device";
        return strMsg;
    }
    return strMsg;
}

int MVSCamera::openDevice(){
    if (true == m_bOpenDevice)
    {
        return STATUS_ERROR;
    }
    int nIndex = 0;//目前只有一个设备，故直接使用0

    // ch:由设备信息创建设备实例 | en:Device instance created by device information
    if (nullptr == m_stDevList.pDeviceInfo[nIndex])
    {
        qDebug() << "Device does not exist" << endl;
        return STATUS_ERROR;
    }

    if (nullptr != m_pcMyCamera)
    {
        qDebug() << "m_pcMyCamera is not null" << endl << "clear m_pcMyCamera!!!";
//        delete[] m_pcMyCamera;
//        m_pcMyCamera = nullptr;
//        return STATUS_ERROR;
    }

    m_pcMyCamera = new CMyCamera;
    if (nullptr == m_pcMyCamera)
    {
        qDebug() << "null m_pcMyCamera" << endl;
        return STATUS_ERROR;
    }

    int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
    if (MV_OK != nRet)
    {
        qDebug() << "Device open failed" << endl;
        delete m_pcMyCamera;
        m_pcMyCamera = nullptr;
        return nRet;
    }

    // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
    if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
    {
        int nPacketSize = m_pcMyCamera->GetOptimalPacketSize();
        if (nPacketSize > 0)
        {
            nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize",nPacketSize);
            if(nRet != MV_OK)
            {
//                ShowErrorMsg(TEXT("Warning: Set Packet Size fail!"), nRet);
            }
        }
        else
        {
//            ShowErrorMsg(TEXT("Warning: Get Packet Size fail!"), nPacketSize);
        }
    }

    m_bOpenDevice = true;

    return MV_OK;
}

// ch:关闭设备 | en:Close Device
int MVSCamera::closeDevice(void)
{
    if (m_pcMyCamera)
    {
        m_pcMyCamera->Close();
        delete m_pcMyCamera;
        m_pcMyCamera = nullptr;
    }

    m_bOpenDevice = false;
    m_bStartGrabbing = false;

    if (m_pBufForDriver)
    {
        free(m_pBufForDriver);
        m_pBufForDriver = nullptr;
    }
    m_nBufSizeForDriver = 0;

    if (m_pBufForSaveImage)
    {
        free(m_pBufForSaveImage);
        m_pBufForSaveImage = nullptr;
    }
    m_nBufSizeForSaveImage  = 0;

    return MV_OK;
}

// ch:获取触发模式 | en:Get Trigger Mode
int MVSCamera::getTriggerMode(void)
{
    QString strFeature;
    unsigned int nEnumValue = 0;

    int nRet = m_pcMyCamera->GetEnumValue("TriggerMode", &nEnumValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    m_nTriggerMode = nEnumValue;
    if (MV_TRIGGER_MODE_ON ==  m_nTriggerMode)
    {
//        OnBnClickedTriggerModeRadio();1
    }
    else if (MV_TRIGGER_MODE_OFF == m_nTriggerMode)
    {
//        OnBnClickedContinusModeRadio();
    }
    else
    {
        return STATUS_ERROR;
    }

    return MV_OK;
}

// ch:设置触发模式 | en:Set Trigger Mode
int MVSCamera::setTriggerMode(void)
{
    int nRet = m_pcMyCamera->SetEnumValue("TriggerMode", m_nTriggerMode);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    return MV_OK;
}

// ch:获取曝光时间 | en:Get Exposure Time
int MVSCamera::getExposureTime(void)
{
    float  fFloatValue = 0.0;
    int nRet = m_pcMyCamera->GetFloatValue("ExposureTime", &fFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    qDebug() << "Getting exposureTime, m_dExposureEdit = " << fFloatValue;
    m_dExposureEdit = fFloatValue;

    return MV_OK;
}

// ch:设置曝光时间 | en:Set Exposure Time
int MVSCamera::setExposureTime(void)
{
    // ch:调节这两个曝光模式，才能让曝光时间生效
    // en:Adjust these two exposure mode to allow exposure time effective
    int nRet = m_pcMyCamera->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    nRet = m_pcMyCamera->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

    qDebug() << "Setting exposureTime, m_dExposureEdit = " << m_dExposureEdit;
    nRet = m_pcMyCamera->SetFloatValue("ExposureTime", m_dExposureEdit);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    return MV_OK;
}

// ch:获取增益 | en:Get Gain
int MVSCamera::getGain(void)
{
    float  fFloatValue = 0.0;

    int nRet = m_pcMyCamera->GetFloatValue("Gain", &fFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }


    qDebug() << "Getting Gain, m_dGainEdit = " << m_dGainEdit;
    m_dGainEdit = (int)fFloatValue;

    return MV_OK;
}

// ch:设置增益 | en:Set Gain
int MVSCamera::setGain(void)
{
    // ch:设置增益前先把自动增益关闭，失败无需返回
    //en:Set Gain after Auto Gain is turned off, this failure does not need to return
    int nRet = m_pcMyCamera->SetEnumValue("GainAuto", 0);

    qDebug() << "Setting Gain, m_dGainEdit = " << m_dGainEdit;
    return m_pcMyCamera->SetFloatValue("Gain", m_dGainEdit);
}

// ch:获取帧率 | en:Get Frame Rate
int MVSCamera::getFrameRate(void)
{

    float  fFloatValue = 0.0;

    int nRet = m_pcMyCamera->GetFloatValue("ResultingFrameRate", &fFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    qDebug() << "Getting FrameRate, m_dFrameRateEdit = " << m_dFrameRateEdit;
    m_dFrameRateEdit = fFloatValue;

    return MV_OK;
}

// ch:设置帧率 | en:Set Frame Rate
int MVSCamera::setFrameRate(void)
{
    int nRet = m_pcMyCamera->SetBoolValue("AcquisitionFrameRateEnable", true);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    qDebug() << "Setting AcquisitionFrameRate, m_dFrameRateEdit = " << m_dFrameRateEdit;
    return m_pcMyCamera->SetFloatValue("AcquisitionFrameRate", m_dFrameRateEdit);
}

// ch:获取触发源 | en:Get Trigger Source
int MVSCamera::getTriggerSource(void)
{
    unsigned int nEnumValue = 0;

    int nRet = m_pcMyCamera->GetEnumValue("TriggerSource", &nEnumValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    if ((unsigned int)MV_TRIGGER_SOURCE_SOFTWARE == nEnumValue)
    {
        m_bSoftWareTriggerCheck = true;
    }
    else
    {
        m_bSoftWareTriggerCheck = false;
    }

    return MV_OK;
}

// ch:设置触发源 | en:Set Trigger Source
int MVSCamera::setTriggerSource(void)
{
    if (m_bSoftWareTriggerCheck)
    {
        m_nTriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
        int nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
        if (MV_OK != nRet)
        {
//            ShowErrorMsg(TEXT("Set Software Trigger Fail"), nRet);
            return nRet;
        }
//        GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(TRUE);
    }
    else
    {
        m_nTriggerSource = MV_TRIGGER_SOURCE_LINE0;
        int nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
        if (MV_OK != nRet)
        {
//            ShowErrorMsg(TEXT("Set Hardware Trigger Fail"), nRet);
            return nRet;
        }
//        GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(false);
    }

    return MV_OK;
}


// ch:保存图片 | en:Save Image
int MVSCamera::saveImage()
{
    if (false == m_bStartGrabbing)
    {
        return STATUS_ERROR;
    }

    // ch:获取1张图 | en:get one image
    unsigned int nRecvBufSize = 0;
    int nRet = MV_OK;

    // ch:仅在第一次保存图像时申请缓存，在 CloseDevice 时释放
    // en:Request buffer only when save image for first time, release after CloseDevice
    if (nullptr == m_pBufForDriver)
    {
        // ch:从相机中获取一帧图像大小 | en:Get size of one frame from camera
        nRet = m_pcMyCamera->GetIntValue("PayloadSize", &nRecvBufSize);
        if (nRet != MV_OK)
        {
            qDebug("failed in get PayloadSize");
            return nRet;
        }
        // ch:一帧数据大小
        // en:One frame size
        m_nBufSizeForDriver = nRecvBufSize;
        m_pBufForDriver = (unsigned char *)malloc(m_nBufSizeForDriver);
        if (nullptr == m_pBufForDriver)
        {
            qDebug("malloc m_pBufForDriver failed, run out of memory");
            return nRet;
        }
    }

    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));

    unsigned int nDataSize = nRecvBufSize;
    unsigned int nImageNum = 1;
    unsigned int nDataLen = 0;

    while(nImageNum)
    {
        nRet = m_pcMyCamera->GetOneFrameTimeout(m_pBufForDriver, &nDataLen, m_nBufSizeForDriver, &stImageInfo, 1000);
        if (nRet == MV_OK)
        {
            nImageNum--;

            // ch:仅在第一次保存图像时申请缓存，在 CloseDevice 时释放
            // en:Request buffer only when save image for first time, release after CloseDevice
            if (nullptr == m_pBufForSaveImage)
            {
                // ch:BMP图片大小：width * height * 3 + 2048(预留BMP头大小)
                // en:// BMP image size: width * height * 3 + 2048 (Reserved BMP header size)
                m_nBufSizeForSaveImage = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;

                m_pBufForSaveImage = (unsigned char*)malloc(m_nBufSizeForSaveImage);
                if (nullptr == m_pBufForSaveImage)
                {
                    break;
                }
            }
            // ch:设置对应的相机参数 | en:Set camera parameter
            MV_SAVE_IMAGE_PARAM_EX stParam = {0};
            stParam.enImageType = m_nSaveImageType; // ch:需要保存的图像类型 | en:Image format to save
            stParam.enPixelType = stImageInfo.enPixelType;  // ch:相机对应的像素格式 | en:Camera pixel type
            stParam.nWidth      = stImageInfo.nWidth;         // ch:相机对应的宽 | en:Width
            stParam.nHeight     = stImageInfo.nHeight;          // ch:相机对应的高 | en:Height
            stParam.nDataLen    = stImageInfo.nFrameLen;
            stParam.pData       = m_pBufForDriver;
            stParam.pImageBuffer = m_pBufForSaveImage;
            stParam.nBufferSize = m_nBufSizeForSaveImage;  // ch:存储节点的大小 | en:Buffer node size
            stParam.nJpgQuality     = 80;       // ch:jpg编码，仅在保存Jpg图像时有效。保存BMP时SDK内忽略该参数
                                                // en:jpg encoding, only valid when saving as Jpg. SDK ignore this parameter when saving as BMP

            nRet = m_pcMyCamera->SaveImage(&stParam);
            if(MV_OK != nRet)
            {
                break;
            }

            char chImageName[IMAGE_NAME_LEN] = {0};
            if (MV_Image_Bmp == stParam.enImageType)
            {
                sprintf_s(chImageName, IMAGE_NAME_LEN, "Image_w%d_h%d_fn%03d.bmp", stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
            }
            else if (MV_Image_Jpeg == stParam.enImageType)
            {
                sprintf_s(chImageName, IMAGE_NAME_LEN, "Image_w%d_h%d_fn%03d.jpg", stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
            }

            qDebug(chImageName);
            FILE* fp = fopen(chImageName, "wb");
            if (nullptr == fp)
            {
                qDebug("write image failed, maybe you have no privilege");
                return STATUS_ERROR;
            }
            fwrite(m_pBufForSaveImage, 1, stParam.nImageLen, fp);
            fclose(fp);
        }
        else
        {
            break;
        }
    }

    qDebug("SaveImage over");
    return nRet;
}

void MVSCamera::continusMode(){
    m_nTriggerMode = MV_TRIGGER_MODE_OFF;
    int nRet = setTriggerMode();
    return;
}


void MVSCamera::triggerMode(){
    m_nTriggerMode = MV_TRIGGER_MODE_ON;
    int nRet = setTriggerMode();
    return;
}

void MVSCamera::startGrabbing(){

    //test, try to use GetOneFrameTimeOut, fail in MV_CC_GetOneFrameTimeout()
//    m_bStartGrabbing = true;
//    getImageBuffer();

    if (false == m_bOpenDevice || true == m_bStartGrabbing)
    {
        qDebug() << "parameter false" << endl;
        qDebug() << "m_bOpenDevice: " << m_bOpenDevice << endl <<"m_bStartGrabbing: " << m_bStartGrabbing << endl;
        return;
    }

    int nRet = MV_OK;
    if (nullptr != m_pcMyCamera)
    {
        qDebug() << "real startGrabbing" << endl;
        nRet = m_pcMyCamera->StartGrabbing();
        if (nRet == MV_OK)
        {
//            nRet = m_pcMyCamera->Display(m_hwndDisplay);
        }
    }
    else
    {
        qDebug() << "null m_pcMyCamera";
        return ;
    }

    if (MV_OK != nRet)
    {
        qDebug() << "startGrabbing failed" << endl;
        return ;
    }
    m_bStartGrabbing = true;

    qDebug("m_bStartGrabbing: %d", m_bStartGrabbing);
    qDebug() << "after startGrabbing, check top 10 chars in buffer: ";
//    for(int i = 0; i < 10; i++){
//        qDebug() << m_pBufForDriver[i] << "   ///" << endl;
//    }

    //add getImageBuffer
    getImageBuffer();

    return;
}

void MVSCamera::stopGrabbing()
{
    if (false == m_bOpenDevice || false == m_bStartGrabbing)
    {
        return;
    }

    int nRet = MV_OK;
    if (nullptr != m_pcMyCamera)
    {
        nRet = m_pcMyCamera->StopGrabbing();
    }
    else
    {
        return ;
    }

    if (MV_OK != nRet)
    {
        return ;
    }
    m_bStartGrabbing = false;
//    EnableControls(TRUE);

    return;
}

void MVSCamera::softwareTrigger(){
//    UpdateData(TRUE);

    int nRet = setTriggerSource();
    if (nRet != MV_OK)
    {
        return;
    }

    return;
}

void MVSCamera::softwareOnce(){
    if (true != m_bStartGrabbing)
    {
        return;
    }

    int nRet = m_pcMyCamera->CommandExecute("TriggerSoftware");

    return;
}

void MVSCamera::saveBmp()
{
    m_nSaveImageType = MV_Image_Bmp;
    int nRet = saveImage();
    if (MV_OK != nRet)
    {
        qDebug("Save bmp fail");
        return;
    }
    qDebug("Save bmp succeed");

    return;
}

// ch:按下获取参数按钮 | en:Click Get Parameter button
void MVSCamera::getParameter()
{
    int nRet = getTriggerMode();
    if (nRet != MV_OK)
    {
//        ShowErrorMsg(TEXT("Get Trigger Mode Fail"), nRet);
    }

    nRet = getExposureTime();
    if (nRet != MV_OK)
    {
        qDebug("Get Exposure Time Fail");
    }

    nRet = getGain();
    if (nRet != MV_OK)
    {
        qDebug("Get Gain Fail");
    }

    nRet = getFrameRate();
    if (nRet != MV_OK)
    {
        qDebug("Get Frame Rate Fail");
    }

    nRet = getTriggerSource();
    if (nRet != MV_OK)
    {
        qDebug("Get Trigger Source Fail");
    }

    qDebug() << "get parameters succeed";
    return;
}

// ch:按下设置参数按钮 | en:Click Set Parameter button
void MVSCamera::setParameter()
{
//    UpdateData(TRUE);

    bool bIsSetSucceed = true;
    int nRet = setExposureTime();
    if (nRet != MV_OK)
    {
        bIsSetSucceed = false;
        qDebug() << "Set Exposure Time Fail";
    }
    nRet = setGain();
    if (nRet != MV_OK)
    {
        bIsSetSucceed = false;
        qDebug("Set Gain Fail");
    }
    nRet = setFrameRate();
    if (nRet != MV_OK)
    {
        bIsSetSucceed = false;
        qDebug("Set Frame Rate Fail");
    }

    if (true == bIsSetSucceed)
    {
        qDebug("Set Parameter Succeed");

    }
    return;
}

void MVSCamera::setParameterQml(double expoTime, double gain, double frmRate)
{
    m_dExposureEdit = expoTime;
    m_dGainEdit = gain;
    m_dFrameRateEdit = frmRate;
    return;
}

double MVSCamera::getExposureTimeQml()
{
    return m_dExposureEdit;
}

double MVSCamera::getGainQml()
{
    return m_dGainEdit;
}

double MVSCamera::getFrameRateTimeQml()
{
    return m_dFrameRateEdit;
}

int MVSCamera::getImageBuffer(){
    if (false == m_bStartGrabbing)
    {
        qDebug() << "m_bStartGrabbing is false";
        return STATUS_ERROR;
    }

    // ch:获取1张图 | en:get one image
    unsigned int nRecvBufSize = 0;
    int nRet = MV_OK;

    // ch:仅在第一次保存图像时申请缓存，在 CloseDevice 时释放
    // en:Request buffer only when save image for first time, release after CloseDevice
    if (nullptr == m_pBufForDriver)
    {
        // ch:从相机中获取一帧图像大小 | en:Get size of one frame from camera
        nRet = m_pcMyCamera->GetIntValue("PayloadSize", &nRecvBufSize);
        if (nRet != MV_OK)
        {
            qDebug("failed in get PayloadSize");
            return nRet;
        }
        // ch:一帧数据大小
        // en:One frame size
        m_nBufSizeForDriver = nRecvBufSize;
        m_pBufForDriver = (unsigned char *)malloc(m_nBufSizeForDriver);
        if (nullptr == m_pBufForDriver)
        {
            qDebug("malloc m_pBufForDriver failed, run out of memory");
            return nRet;
        }
    }

    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));

    unsigned int nDataLen = 0;

    nRet = m_pcMyCamera->GetOneFrameTimeout(m_pBufForDriver, &nDataLen, m_nBufSizeForDriver, &stImageInfo, 1000);
    qDebug() << "GetOneFrameTimeOut result is "<< nRet + 1 << endl;

    m_nWidth      = stImageInfo.nWidth;         // ch:相机对应的宽 | en:Width
    m_nHeight     = stImageInfo.nHeight;          // ch:相机对应的高 | en:Height

    qDebug() << "width: " << m_nWidth << endl
             << "height: " << m_nHeight << endl
             << "buffer size: " << m_nBufSizeForDriver << endl
             << "top 10 chars in buffer: ";
    for(int i = 0; i < 10; i++){
        qDebug() << m_pBufForDriver[i] << " /// ";
    }

//    int nRet = 1;
//    for(int i = 0; i < 10; i++){
//        qDebug() << m_pBufForDriver[i] << " /// ";
//    }

    return nRet;
}
