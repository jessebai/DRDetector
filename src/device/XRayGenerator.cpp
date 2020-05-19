#include "XRayGenerator.h"

XRayGenerator::XRayGenerator() : SerialDevice ()
{
//    initPort("COM3");
////    qDebug()<<"initializing COM3";
//    connect(&m_curSerial, &QSerialPort::readyRead, this, &XRayGenerator::handleReadyRead);
//    connect(&m_curSerial, &QSerialPort::errorOccurred, this, &SerialDevice::handleError);
//    connect(&m_cmdCycleTimer, &QTimer::timeout, this, &XRayGenerator::messageQueue);

//    if(m_curSerial.isOpen()){
//        m_cmdCycleTimer.start(200);
//    }

}

void XRayGenerator::messageQueue(){
    if(m_bIsPortBusy)
        return;
    if(m_commandQueue.empty()){
        //commands that are sent continuously by turns.
        m_curCommand = m_iConstCommIdx;
        sendCommand(buildSendData(m_constantCommands[m_iConstCommIdx]));

        if(++m_iConstCommIdx >= m_iConstCommCount)
            m_iConstCommIdx = 0;
    }else{
        m_curCommand = ACT;
        sendCommand(m_commandQueue.front());
        m_commandQueue.pop();
    }
}


void XRayGenerator::sendCommand(QByteArray command){
    //qDebug()<<command;
    m_bIsPortBusy = true;
    m_readData.clear();
    m_curSerial.write(command);
    m_curSerial.flush();
}

QByteArray XRayGenerator::buildSendData(QByteArray command){
    return buildSendData(command, QByteArray());
}


QByteArray XRayGenerator::buildSendData(QByteArray command, QByteArray argument){
    QByteArray sendData;
    sendData.append(STX);
    sendData.append(command);
    if(!argument.isEmpty()){
        sendData.append(argument);
    }
    sendData.append(CR);
    return sendData;
}

void XRayGenerator::setVoltage(unsigned int voltage){
    m_iVolSetting = voltage;
    QByteArray volStr;
    volStr.setNum(voltage);
    QByteArray sendData = buildSendData("VP", volStr.rightJustified(4, '0'));
    m_commandQueue.push(sendData);
}

void XRayGenerator::setCurrent(unsigned int current){
    m_iCurSetting = current;
    QByteArray curStr;
    curStr.setNum(current);

    QByteArray sendData = buildSendData("CP", curStr.rightJustified(5, '0'));
    m_commandQueue.push(sendData);
}

void XRayGenerator::faultClear(){
    QByteArray sendData = buildSendData("CLR");
    m_commandQueue.push(sendData);
}

void XRayGenerator::enableXRay(){
    QByteArray sendData = buildSendData("ENBL","1");
    m_commandQueue.push(sendData);
    m_bIsPowerUp = true;
}

void XRayGenerator::disableXRay(){
    QByteArray sendData = buildSendData("ENBL", "0");
    m_commandQueue.push(sendData);
}

void XRayGenerator::getFaultReport(){
    QByteArray sendData = buildSendData("FLT");
    sendCommand(sendData);

}

QByteArray XRayGenerator::getDeviceParam(){
    QByteArray sendData = buildSendData("MON");
    return sendData;
}

void XRayGenerator::getModeNum(){
    QByteArray sendData = buildSendData("MNUM");
    sendCommand(sendData);
}

void XRayGenerator::getHVStatus(){
    QByteArray sendData = buildSendData("STAT");
    sendCommand(sendData);
}

void XRayGenerator::getWatchDogStatus(){
    QByteArray sendData = buildSendData("WSTAT");
    sendCommand(sendData);
}

void XRayGenerator::getMinimizationStatus(){
    QByteArray sendData = buildSendData("MSTAT");
    sendCommand(sendData);
}

void XRayGenerator::setWatchDogTimer(){
    QByteArray sendData = buildSendData("OK");
    sendCommand(sendData);
}

void XRayGenerator::enableWatchDog(){
    QByteArray sendData = buildSendData("WDOG","1");
    sendCommand(sendData);
}

void XRayGenerator::disableWatchDog(){
    QByteArray sendData = buildSendData("WDOG","0");
    sendCommand(sendData);
}

void XRayGenerator::test(){
    disableWatchDog();
    setVoltage(600); //必须四位， 包含一位小数
    setCurrent(1000);//必须五位，整数
    enableXRay();
    return;
    //getHVStatus();
    //getDeviceParam();
    //disableXRay();
    //getDeviceParam();
}


void XRayGenerator::getStatus(){
    initPort("COM4");
    QByteArray status;
//    status.append(getHVStatus());
//    status.append('\n');
//    status.append(getWatchDogStatus());
//    status.append('\n');
//    status.append(getFaultReport());
//    status.append('\n');
//    status.append(getDeviceParam());
//    status.append('\n');
    //status.append(getMinimizationStatus());
    qDebug()<<status;
}

void XRayGenerator::close(){
    initPort("COM3");
    //qDebug()<<disableXRay();
}

void XRayGenerator::handleReadyRead(){
    m_readData.append(m_curSerial.readAll());
    if(m_readData.contains('\r')){
        m_bIsPortBusy = false;
        m_readData.chop(1);
        m_readData.remove(0,1);
        //qDebug()<<"response:"<<m_readData;
        switch(m_curCommand){
            case MON:{
                QList<QByteArray> monInfos = m_readData.split(' ');
                unsigned int curVol = monInfos.at(0).toUInt();
                unsigned int curCur = monInfos.at(1).toUInt();
                if(m_bIsPowerUp && curVol > 0.9 * m_iVolSetting && curCur> 0.9 * m_iCurSetting){
                    emit fullPower();
                    m_bIsPowerUp = false;
                }
                break;
            }
            case FLT:{
                //qDebug()<<"fault list"<< m_readData;
                break;
            }
            case STAT:{
                //qDebug()<<"Xray status"<<m_readData;
                break;
            }
            case ACT:{
                //qDebug()<<m_readData;

                break;
            }
        }
        m_readData.clear();
    }
}
