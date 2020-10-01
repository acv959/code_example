#include "TimerINSPERT.h"

TimerINSPERT::TimerINSPERT(QMutex *mutex):_mutex(mutex){

    connect(&_thread, SIGNAL(started() ),
            this, SLOT(started() ) );
}

void TimerINSPERT::start(int msec){

    _msec = msec;
    this->moveToThread(&_thread);

    _thread.start();

    return;
}


TimerINSPERT::~TimerINSPERT(){

    QTimer::stop();
    _thread.quit();
}


bool TimerINSPERT::event(QEvent *event){

    QMutexLocker ml(_mutex);
    return QObject::event(event);
}

void TimerINSPERT::started(){

    QTimer::start(_msec);
    return;
}
