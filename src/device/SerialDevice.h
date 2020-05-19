#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QDebug>
#include <QString>
#include <QList>
#include <QJsonArray>
#include <QDateTime>
#include <QThread>
#include <QTimer>


class SerialDevice : public QObject
{
    Q_OBJECT
public:
    SerialDevice(QObject *parent = nullptr);
    ~SerialDevice() {}

    void initPort(QString portName);


    void detectPorts();
    QJsonArray getPortsList();

    void sendCommand(QByteArray command);

    void handleReadyRead();

    void handleBytesWritten();

    void handleError(QSerialPort::SerialPortError serialPortError);

    QSerialPort m_curSerial;
private:
    QList<QSerialPortInfo> m_serialPortList;

};

#endif // SERIALDEVICE_H
