#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <dos.h>
#include "thread.h"
#include "pcb.h"

#define lock() { Global::lockFlag++;}
#define unlock() { Global::lockFlag--;}

class Global{
private:
    friend class Thread;
    friend class PCB;
    friend class Queue;
    friend class IdleThread;
    static void interrupt (*oldTimerInterrupt)(...);
    static void interrupt timerInterrupt(...);
    static volatile int contextSwitchOnDemand;
public:
    static volatile int lockFlag;
    static void initialize();
    static void finalize();
    static void dispatch();
};

extern int tick();

#endif