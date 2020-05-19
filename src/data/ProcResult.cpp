#include "ProcResult.h"

ProcResult::ProcResult()
{

}

ProcResult::ProcResult(QByteArray num, QByteArray flags){
    count = num;
    bottleFlags = flags;
}


QByteArray ProcResult::getCount(){
    return count;
}

QByteArray ProcResult::getBottles(){
    return bottleFlags;
}
