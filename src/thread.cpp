#include "thread.h"
#include "queue.h"
#include "schedule.h"
#include "global.h"
#include "idle.h"
#include <dos.h>
#include <stdlib.h>

class Global;

int Thread::_ID=0;
Thread* Thread::mainThread=NULL;
volatile Thread* Thread::running=NULL;
Queue Thread::allThreads;

Thread::Thread (StackSize stackSize, Time timeSlice){
    lock();
    id=++_ID;
    cout<<"stsize "<<stackSize<<endl; 
    myPCB=new PCB(this, stackSize, timeSlice);
    allThreads.put(this);
    waitingThreads=new Queue;
    unlock();
}

Thread::~Thread(){
    lock();
    waitToComplete();
    delete waitingThreads;
    delete myPCB;
    unlock();
}

ID Thread::getId(){
    return id;
}

ID Thread::getRunningId(){
    return running->getId();
}

Thread* Thread::getThreadById(ID id){
    return allThreads.findByID(id);
}

void Thread::waitToComplete(){
    lock();
    if(myPCB->state==PCB::OVER || myPCB->state==PCB::NEW || this==running || this==mainThread || this==IdleThread::getIdle()){
        unlock();
        return;
    }
    running->myPCB->state=PCB::BLOCKED;
    waitingThreads->put((Thread*)running);
    dispatch();
    unlock();
}

void Thread::start(){
    lock();
    if(myPCB->state!=PCB::NEW){
        unlock();
        return;
    }
    myPCB->initStack();
    myPCB->state=PCB::READY;
    Scheduler::put(myPCB);
    unlock();
}

void dispatch(){
    Global::dispatch();
}