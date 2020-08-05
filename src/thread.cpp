#include "thread.h"
#include "queue.h"
#include "schedule.h"
#include "global.h"
#include "idle.h"
#include "mainthr.h"
#include <dos.h>
#include <stdlib.h>

class Global;

int Thread::_ID=0;
volatile Thread* Thread::running=NULL;
Queue Thread::allThreads;

Thread::Thread (StackSize stackSize, Time timeSlice){
    lock();
    id=++_ID;
    myPCB=new PCB(this, stackSize, timeSlice);
    allThreads.put(this);
    waitingThreads=new Queue;
    unlock();
}

Thread::~Thread(){
    waitToComplete();
    lock();
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
    if(myPCB->state!=PCB::OVER && myPCB->state!=PCB::NEW && this!=running && this!=MainThread::getMain() && this!=IdleThread::getIdle()){
        running->myPCB->state=PCB::BLOCKED;
        waitingThreads->put((Thread*)running);
        printDebug("waittocomplete: pozvan za "<<getId()<<" pozivalac "<<getRunningId()<<" lockflag "<<Global::lockFlag);
        unlock();
        dispatch();
    }
    else unlock();
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

void Thread::run(){
    lock();
    printDebug("Usao u prazan run Thread klase");
    unlock();
}