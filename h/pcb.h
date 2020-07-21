#ifndef _PCB_H_
#define _PCB_H_

#include "thread.h"

class Thread;

typedef unsigned long StackSize;
typedef unsigned int Time; // time, x 55ms

class PCB{
private:   
    friend class Thread;
    friend class Global;
    friend class IdleThread;
    Thread* myThread;
    static const int NEW, READY, BLOCKED, OVER;
    volatile int state;
    StackSize stackSize;
    Time timeSlice; //obratiti paznju kasnije
    volatile Time timeElapsed;
    unsigned* stack;
    unsigned sp, ss;
    void initStack();
public:
    PCB(Thread*, StackSize, Time);
    virtual ~PCB();
    static void wrapper();
};

#endif