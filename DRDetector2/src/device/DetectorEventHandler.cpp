#include "DetectorEventHandler.h"

void DetectorEventHandler::Process(int nEventID, CR_Event* pEventParam){
    qDebug()<<"event ID:"<<nEventID;
    switch(nEventID){
    case CR_EVT_NEW_FRAME:
        {
            int nBuffIdx = *(int*)pEventParam->pData;
            qDebug()<<"new frame event";
            emit newFrameIdx(nBuffIdx);
            break;
        }

    case CR_EVT_CALIBRATION_FINISHED:
        {
            emit calibrationFinish();
        }
    }
}
