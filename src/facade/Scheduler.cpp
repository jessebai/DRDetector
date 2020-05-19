#include "Scheduler.h"

Scheduler::Scheduler(QObject *parent): QObject(parent)
{
    std::thread initThread(&Scheduler::transferResultCycle, this);
    initThread.detach();
}

void Scheduler::pushResult(QByteArray count, QByteArray bottleFlags){
    qDebug()<<"procResult:"<<count<<bottleFlags;
    m_resultQueue.push(new ProcResult(count, bottleFlags));
}


void Scheduler::transferResult(){
    ProcResult* front = m_resultQueue.front();
    m_resultQueue.pop();
//    plc->setFilterMission(front->getCount(), front->getBottles());
    delete front;
}

void Scheduler::transferResultCycle(){
    while(true){
        //qDebug()<<"filter mission:"<<m_resultQueue.size();
        if(!m_resultQueue.empty()){
            if(m_bIsFilterReady){
                ProcResult* front = m_resultQueue.front();
                m_resultQueue.pop();
                emit sendFilterMission(front->getCount(), front->getBottles());
                m_bIsFilterReady = false;
                delete front;
            }
        }
    }
}

void Scheduler::setFilterReady()
{
    m_bIsFilterReady = true;
}
