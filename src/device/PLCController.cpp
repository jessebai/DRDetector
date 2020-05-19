#include "PLCController.h"

PLCController::PLCController() : SerialDevice (){
//    initPort("COM5");
////    qDebug()<<"initializing COM5";
//    connect(&m_curSerial, &QSerialPort::readyRead, this, &PLCController::handleReadyRead);
//    //connect(&m_curSerial, &QSerialPort::bytesWritten, this, &SerialDevice::handleBytesWritten);
//    connect(&m_curSerial, &QSerialPort::errorOccurred, this, &SerialDevice::handleError);

//    connect(&m_cmdCycleTimer, &QTimer::timeout, this, &PLCController::messageQueue);

//    //if(m_curSerial.isOpen()){
//        m_cmdCycleTimer.start(200);
//   // }

}

PLCController::~PLCController(){

}

void PLCController::readRegister(QByteArray memeoryArea, QByteArray beginAddr, QByteArray elementsNum){
    QByteArray command;
    command.append(m_head);
    command.append(READCODE);
    command.append(memeoryArea);
    command.append(beginAddr);
    command.append(elementsNum);
    m_FCS = CountFCS(command);
    command.append(m_FCS).append(END);
    sendCommand(command);
}

void PLCController::writeRegister(QByteArray memeoryArea, QByteArray beginAddr, QByteArray elementsNum, QByteArray value){
    m_curCommType = WriteReg;
    QByteArray command = buildWriteCommand(memeoryArea, beginAddr, elementsNum, value);
    sendCommand(command);
}

QByteArray PLCController::buildWriteCommand(QByteArray memeoryArea, QByteArray beginAddr, QByteArray elementsNum, QByteArray value){
    m_curCommType = WriteReg;
    QByteArray command;
    command.append(m_head);
    command.append(WRITECODE);
    command.append(memeoryArea);
    command.append(beginAddr);
    command.append(elementsNum);
    command.append(value);
    m_FCS = CountFCS(command);
    command.append(m_FCS).append(END);
    return command;
}

void PLCController::startPipeline(){
//    qDebug()<<"setting w0.03";

    m_commandQueue.push(buildWriteCommand(WR, "000003", "0001", "01"));

    m_commandQueue.push(buildWriteCommand(WR, "000003", "0001", "00"));


//    QTimer::singleShot(200, this, std::bind(&PLCController::writeRegister, this, WR, "000003", "0001", "00"));
}

void PLCController::stopPipeline(){
    m_commandQueue.push(buildWriteCommand(WR, "000004", "0001", "01"));

    m_commandQueue.push(buildWriteCommand(WR, "000004", "0001", "00"));

}

//write D0000 register with the number of bottles in the last cycle
//reset w0.01 to resume the pipeline
void PLCController::setNextCycleCounter(QByteArray count){
//    qDebug()<<"setting next cycle"<<count;
    m_commandQueue.push(buildWriteCommand(DM, "000000", "0001", count));
    m_commandQueue.push(buildWriteCommand(WR, "000001", "0001", "00"));
}

//write D0012 with item count
//write D0013-D0020 with bottle flags
//reset w0.05, filter ready register
void PLCController::setFilterMission(QByteArray count, QByteArray bottles){
//    qDebug()<<"setting filter mission";
    m_commandQueue.push(buildWriteCommand(DM, "000C00", "0001", count));
    m_commandQueue.push(buildWriteCommand(DM, "000D00", count, bottles));
    m_commandQueue.push(buildWriteCommand(WR, "000005", "0001", "00"));
}


//instant messages: command triggered by UI controls
//continuous message: command that need to be sent all the time
//the serial port has limited capability on singal transfer.
//this function decides which command to send at each communication cycle
//when there is instant message queueing, send instant command first
//when there is no instant message, send continous messages by turns.
void PLCController::messageQueue(){
    if(!m_commandQueue.empty()){
//        qDebug(m_commandQueue.front());
        sendCommand(m_commandQueue.front());
        m_commandQueue.pop();
    }else{
        if(m_bTargetOrFilter){
            readRegister(WR, "000001", "0001");
            m_bTargetOrFilter = !m_bTargetOrFilter;
        }else{
            readRegister(WR, "000005", "0001");
            m_bTargetOrFilter = !m_bTargetOrFilter;
        }
    }

}

void PLCController::sendCommand(QByteArray command){

    //qDebug()<<"command sent:PLCController"<<command;

    m_curSerial.write(command);
    m_curSerial.flush();


}


void PLCController::test(){
    //writeRegister(DM, "000C00", "0001", "0004");
    //writeRegister(DM, "000C00", "0001", "0004");
}

void PLCController::handleReadyRead(){
    m_readData = m_curSerial.readAll();
    //qDebug()<<"read ready"<<m_readData;

    if(m_bTargetOrFilter){
        bool b_old = m_bIsFilterReady;
        m_bIsFilterReady = m_readData.at(WR_VALUE_POS) == ISTRUE;
        if(m_bIsFilterReady && !b_old){
            qDebug()<<"filter ready"<<m_bIsFilterReady;
            emit filterReady();
        }
    }else{
        bool b_old = m_bIsTargetReady;
        m_bIsTargetReady = m_readData.at(WR_VALUE_POS) == ISTRUE;
        if(m_bIsTargetReady && !b_old){
            qDebug()<<"target ready"<<m_bIsTargetReady;
            emit targetReady();
        }

    }

}
