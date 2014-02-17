#ifndef CTICKTIMER_H
#define CTICKTIMER_H

#include <Windows.h>
#include <QtGlobal>
#include <QDebug>

class CTickTimer
{
public:
    CTickTimer() { QueryPerformanceFrequency(&_perfFreq); }

    void Start() { QueryPerformanceCounter(&_perfStart); }

    bool Timeout(qint64 ms) {
        LARGE_INTEGER perfNow;
        QueryPerformanceCounter(&perfNow);
        return ((((perfNow.QuadPart - _perfStart.QuadPart) * 1000) / _perfFreq.QuadPart) > ms);
    }

    bool TimeoutMicro(qint64 us) {
        LARGE_INTEGER perfNow;
        QueryPerformanceCounter(&perfNow);
        return ((((perfNow.QuadPart - _perfStart.QuadPart) * 1000000) / _perfFreq.QuadPart) > us);
    }



private:
    LARGE_INTEGER _perfFreq;
    LARGE_INTEGER _perfStart;
};

#endif // CTICKTIMER_H
