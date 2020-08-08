#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <dos.h>
#include "thread.h"
#include "pcb.h"

#define lock() { Global::lockFlag++;}
#define unlock() { Global::lockFlag--; if(Global::lockFlag==0 && Global::contextSwitchOnDemand) Global::dispatch();}
#define MyintLock() {asm{pushf; cli;}}
#define MyintUnlock() {asm{popf}}
#define printDebug(X) {MyintLock(); cout<<X<<endl; MyintUnlock();}


class Global{
private:
    friend class Thread;
    friend class PCB;
    friend class Queue;
    friend class IdleThread;
    static void interrupt (*oldTimerInterrupt)(...);
    static void interrupt timerInterrupt(...);
public:
    static volatile int contextSwitchOnDemand;
    static volatile int lockFlag;
    static void initialize();
    static void finalize();
    static void dispatch();
};

extern void tick();

#endif