#include "idle.h"
#include "thread.h"
#include "global.h"
#include "pcb.h"

volatile Thread* IdleThread::idle=NULL;

IdleThread::IdleThread():Thread(100, 1){}

void IdleThread::start(){
    lock();
    if(myPCB->state!=PCB::NEW){
        unlock();
        return;
    }
    myPCB->initStack();
    myPCB->state=PCB::READY;
    unlock();
}

void IdleThread::run(){
    while(1);
}

Thread* IdleThread::getIdle(){
    if(!idle) idle=new IdleThread;
    return (Thread*)idle;
}

void IdleThread::deleteIdle(){
    delete idle;
    //idle=NULL;
}