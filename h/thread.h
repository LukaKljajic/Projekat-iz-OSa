#ifndef _THREAD_H_
#define _THREAD_H_

#include "global.h"
#include "pcb.h"
#include "queue.h"
#include "ksemlist.h"
#include "event.h"

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;

typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms

typedef int ID;

class PCB; // Kernel's implementation of a user's thread
class Queue;
class Global;

class Thread {
public:
    void start();
    void waitToComplete();
    virtual ~Thread();
    ID getId();
    static ID getRunningId();
    static Thread* getThreadById(ID id);
protected:
    friend class PCB;
    friend class Global;
    friend class IdleThread;
    friend class KernelSem;
    friend class KSemList;
    friend class Event;
    Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
    virtual void run();
private:
    PCB* myPCB;
    static volatile Thread* running;
    static Queue allThreads;
    Queue* waitingThreads;
    static ID _ID;
    ID id;
};

void dispatch();

#endif