#ifndef _IDLE_H_
#define _IDLE_H_

#include "thread.h"
#include "global.h"

class Thread;

class IdleThread:public Thread{
    IdleThread();
    static volatile Thread* idle;
public:
    void start();
    static Thread* getIdle();
    static void deleteIdle();
protected:
    virtual void run();
};

#endif