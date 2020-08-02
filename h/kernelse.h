#ifndef _KERNELSE_H_
#define _KERNELSE_H_

#include "semaphor.h"
#include "global.h"
#include "queue.h"
#include "ksemlist.h"

class Semaphore;
class KSemList;

class KernelSem{
public:
    KernelSem(int init);
    virtual ~KernelSem();
    virtual int wait (Time maxTimeToWait);
    virtual int signal(int n=0);
    int val () const;
private:
    friend class Global;
    static KSemList semaphores;
    Queue* waitingThreads;
    int value;
};

#endif