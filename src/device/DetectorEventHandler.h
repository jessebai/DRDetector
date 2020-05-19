#ifndef DETECTOREVENTHANDLER_H
#define DETECTOREVENTHANDLER_H

#include <QObject>
#include <QDebug>

#include <ICallback.h>


class DetectorEventHandler : public QObject, public ICallback
{
    Q_OBJECT
public:
    //constructor
    explicit DetectorEventHandler(QObject* parent=nullptr):QObject(parent), ICallback(){}

    virtual void Process(int nEventID, CR_Event* pEventParam);

signals:
    void newFrameIdx(int nBuffidx);
    void calibrationFinish();

public slots:
};

#endif // DETECTOREVENTHANDLER_H
