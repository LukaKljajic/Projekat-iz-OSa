#include "kernelse.h"
#include "global.h"
#include "thread.h"
#include "schedule.h"

KernelSem::KernelSem(int init):value(init){
    KSemList::semaphores.put(this);
    waitingThreads=new Queue; 
}

KernelSem::~KernelSem(){
    delete waitingThreads;
}

int KernelSem::wait(Time maxTimeToWait){
    if(--value<0){
        Thread::running->myPCB->state=PCB::BLOCKED;
        Thread::running->myPCB->waitingTime=maxTimeToWait;
        waitingThreads->put((Thread*)Thread::running);
        // printDebug("Blokirao nit "<<Thread:: getRunningId());
        unlock();
        dispatch();
        lock();
    }
    if(Thread::running->myPCB->unblockedByTime){
        Thread::running->myPCB->unblockedByTime=0;
        return 0;
    }
    return 1;
}

int KernelSem::signal(int n){
    if(n<0){
        return n;
    } 
    if(n==0){
        if(++value<=0){
            Thread* t=waitingThreads->get();
            t->myPCB->state=PCB::READY;
            t->myPCB->unblockedByTime=0;
            Scheduler::put(t->myPCB);
            // printDebug("Odblokirao nit "<<t->getId()<<" signalom");
        }
        return 0;
    }
    int ret=0;
    for(;n>0 && value<0; n--, value++){
        Thread* t=waitingThreads->get();
        t->myPCB->state=PCB::READY;
        t->myPCB->unblockedByTime=0;
        Scheduler::put(t->myPCB);
        // printDebug("Odblokirao nit "<<t->getId()<<" signalom");
        ret++;
    }
    value+=n;
    return ret;
}

int KernelSem::val() const {
    return value;
}