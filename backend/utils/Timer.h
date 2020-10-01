#ifndef TIMER_H
#define TIMER_H

#include <mutex>
#include <iostream>
#include <chrono>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

template <typename Object, typename Method>
class Timer
{
    Object      _obj;
    Method      _mtd;
    int         _period;
    pthread_t   *_pthread;


public:
    Timer(Object obj, Method mtd):_obj(obj),_mtd(mtd),_period(1000000),_pthread(NULL) {}

    void start(int period){

        _period = period;
         _pthread = new  pthread_t;
        if((pthread_create(_pthread, NULL, run,this))<0){

            std::string error = "Worker::start: Pthread create :  " + std::string(strerror(errno));
            printf("%s\n",error.c_str());

            return ;
        }

        pthread_detach(*_pthread);
    }



    void stop(){

        if(_pthread ){

            if(  pthread_cancel(*_pthread) < 0){

                std::string error = "Worker::stop: Pthread cancel :  " + std::string(strerror(errno));
                printf("%s\n",error.c_str());

                return ;
            }
            delete _pthread;
        }

        return;
    }


    static void *run(void *param){

        Timer *timer = (Timer *)param;

        while (1) {


            sleep(timer->_period);
            (timer->_obj->*(timer->_mtd))();


        }

        return NULL;
    }
};


#endif // TIMER_H
