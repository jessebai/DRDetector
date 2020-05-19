#ifndef PLCCONTROLLER_H
#define PLCCONTROLLER_H

#include "SerialDevice.h"

#include <QTimer>
#include <QThread>
#include <queue>
#include <mutex>

class PLCController : public SerialDevice
{
    Q_OBJECT
public:
    PLCController();
    ~PLCController();

    enum CommandType{
        TargetCheck,
        FilterCheck,
        WriteReg,
    };

    QByteArray CountFCS(QByteArray command){
        char FCS=command[0];
        for(int i=1;i<command.size();i++){
            FCS=FCS^command[i];
        }
        return QByteArray(1,FCS).toHex().toUpper();
    }

    //write to serial port, no blocking
    void sendCommand(QByteArray command);

    void readRegister(QByteArray memeoryArea, QByteArray beginAddr, QByteArray elementsNum);
    void writeRegister(QByteArray memeoryArea, QByteArray beginAddr, QByteArray elementsNum, QByteArray value);

    QByteArray buildWriteCommand(QByteArray memeoryArea, QByteArray beginAddr, QByteArray elementsNum, QByteArray value);

    Q_INVOKABLE void startPipeline();



    Q_INVOKABLE void stopPipeline();

    Q_INVOKABLE void setNextCycleCounter(QByteArray count);

    //new filter mission written to plc
    Q_INVOKABLE void setFilterMission(QByteArray count, QByteArray bottles);

    //communication implementation with plc at fixed interval
    void messageQueue();

    Q_INVOKABLE void test();

    //process read buffer upon readyread signal from serial port
    void handleReadyRead();

signals:
    void targetReady();
    void filterReady();

private:
    QTimer m_cmdCycleTimer;

    bool m_bTargetOrFilter;

    std::queue<QByteArray> m_commandQueue;

    QByteArray m_retWriteReg;

    bool m_bIsTargetReady = false;
    bool m_bIsFilterReady = false;

    CommandType m_curCommType;

    QByteArray m_readData;


    //constant define, remove magic numbers
    const QByteArray END="*\r";
    const QByteArray READCODE = "0101";
    const QByteArray WRITECODE = "0102";
    const QByteArray WR = "31";
    const QByteArray DM = "82";

    const int WR_VALUE_POS = 24;
    const char ISTRUE = '1';
    const char ISFALSE = '0';

    const int DELAY = 500;    //millisecond

    QByteArray m_head="@00FA000000000";
    QByteArray m_commandCode="0101";
    QByteArray m_memoryArea="82";
    QByteArray m_beginAddr="000100";
    QByteArray m_elementsNum="0001";
    QByteArray m_FCS="7C";
};

#endif // PLCCONTROLLER_H
