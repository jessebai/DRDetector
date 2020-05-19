#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QObject>
#include <QtDebug>

#include <QThread>
#include <thread>
#include <queue>

#include "src/device/PLCController.h"
#include "src/data/ProcResult.h"


class Scheduler : public QObject
{
    Q_OBJECT

public:
    Scheduler(QObject *parent = nullptr);


    //buffer realtime image process results
    Q_INVOKABLE void pushResult(QByteArray count, QByteArray bottleFlags);

    void transferResult();

    void transferResultCycle();


    //setter
    Q_INVOKABLE void setFilterReady();

signals:
    void sendFilterMission(QByteArray count, QByteArray bottleFlags);

private:
    std::queue<ProcResult*> m_resultQueue;

    bool m_bIsFilterReady;

};

#endif // SCHEDULER_H
