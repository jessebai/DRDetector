#include "SerialDevice.h"

SerialDevice::SerialDevice(QObject *parent):QObject(parent)
{


}

void SerialDevice::initPort(QString portName){
    if(m_curSerial.isOpen()){
        m_curSerial.close();
    }
    m_curSerial.setPortName(portName);
    m_curSerial.setBaudRate(QSerialPort::Baud9600);
    m_curSerial.setDataBits(QSerialPort::Data8);
    m_curSerial.setParity(QSerialPort::NoParity);
    m_curSerial.setStopBits(QSerialPort::OneStop);
    m_curSerial.setFlowControl(QSerialPort::NoFlowControl);

//    qDebug()<<m_curSerial.error();
    m_curSerial.open(QIODevice::ReadWrite);

}


void SerialDevice::detectPorts(){
//    qDebug()<<"detect ports";
    m_serialPortList = QSerialPortInfo::availablePorts();
//    qDebug()<<"all ports";
    for(int i=0;i<m_serialPortList.count();i++){
        QSerialPortInfo port = m_serialPortList.at(i);
//        qDebug()<<port.description();
//        qDebug()<<port.portName();
//        qDebug()<<port.manufacturer();
//        qDebug()<<port.serialNumber();
    }
}

QJsonArray SerialDevice::getPortsList(){
    QJsonArray portJson;
    for(int i = 0; i < m_serialPortList.count();i++){
        portJson.append(m_serialPortList.at(i).portName());
    }
    return portJson;
}

void SerialDevice::sendCommand(QByteArray command){

//    qDebug()<<command;
    //m_readData.clear();
    m_curSerial.write(command);
//    qDebug()<<"flush"<<m_curSerial.flush();
//    qDebug()<<QDateTime::currentDateTime();

}

void SerialDevice::handleReadyRead(){
//    qDebug()<<"read ready";
//    qDebug()<<QDateTime::currentDateTime();
    //m_readData = m_curSerial.readAll();
    //qDebug()<<m_readData;

}

void SerialDevice::handleBytesWritten(){
//    qDebug()<<"bytes written";
//    qDebug()<<QDateTime::currentDateTime();
}

void SerialDevice::handleError(QSerialPort::SerialPortError serialPortError){
//   qDebug()<<"error:"<<serialPortError;
}

