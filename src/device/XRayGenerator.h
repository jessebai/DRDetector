#ifndef XRAYGENERATOR_H
#define XRAYGENERATOR_H

#include <QTimer>
#include <queue>
#include <QThread>

#include "SerialDevice.h"


class XRayGenerator : public SerialDevice
{
    Q_OBJECT
public:
    XRayGenerator();

    enum CommandTypes{
        MON,
        FLT,
        STAT,
        ACT,
    };

    Q_INVOKABLE void test();
    Q_INVOKABLE void close();
    Q_INVOKABLE void getStatus();

    void sendCommand(QByteArray command);

    QByteArray buildSendData(QByteArray command);
    QByteArray buildSendData(QByteArray command, QByteArray argument);

    //generator operations
    Q_INVOKABLE void setVoltage(unsigned int voltage);
    Q_INVOKABLE void setCurrent(unsigned int current);
    Q_INVOKABLE void faultClear();
    Q_INVOKABLE void enableXRay();
    Q_INVOKABLE void disableXRay();



    //generator info retrive
    void getFaultReport();
    QByteArray getDeviceParam();
    void getModeNum();   //device model type
    void getHVStatus();  //generator status
    void getWatchDogStatus();
    void getMinimizationStatus();

    Q_INVOKABLE void setWatchDogTimer();
    Q_INVOKABLE void enableWatchDog();
    Q_INVOKABLE void disableWatchDog();

    void messageQueue();

    void handleReadyRead();

signals:
    void fullPower();

private:
    QTimer m_cmdCycleTimer;
    std::queue<QByteArray> m_commandQueue;

    static const int m_iConstCommCount = 3;
    const QByteArray m_constantCommands[m_iConstCommCount]={"MON", "FLT", "STAT"};
    unsigned short m_iConstCommIdx = 0;

    unsigned short m_curCommand;

    //the value specified by user
    unsigned int m_iVolSetting;
    unsigned int m_iCurSetting;

    //some commands(e.g. ENBL) takes very long to respond, exceeding the heartbeat time frame of communication
    //to ensure that all response are complete, the port is working sequentially
    //the portBusy flag indicates some command is pending for response
    //it is set at sendcommand, and is canceled when a complete response is retrieved.
    bool m_bIsPortBusy = false;

    //the xray generator takes time to reach the expected voltage and current
    //when enabling, this flag is set to examine power status.
    bool m_bIsPowerUp = false;


    //message header and tail
    const char STX = 2;
    const char CR = 13;

    QByteArray m_readData;
};
#endif // XRAYGENERATOR_H
