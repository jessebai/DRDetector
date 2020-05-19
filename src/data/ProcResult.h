#ifndef PROCRESULT_H
#define PROCRESULT_H

#include <stdio.h>
#include <QByteArray>

class ProcResult
{
public:
    ProcResult();

    ProcResult(QByteArray num, QByteArray flags);

    QByteArray getCount();

    QByteArray getBottles();
private:
    QByteArray count;
    QByteArray bottleFlags;
};

#endif // PROCRESULT_H
