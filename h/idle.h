#ifndef _IDLE_H_
#define _IDLE_H_

#include "thread.h"
#include "global.h"

class Thread;

class IdleThread:public Thread{
    IdleThread();
    static IdleThread* idle;
public:
    void start();
    static IdleThread* getIdle();
    static void deleteIdle();
protected:
    virtual void run();
};

#endif