#include "pcb.h"
#include "thread.h"
#include "queue.h"
#include "schedule.h"
#include <dos.h>
#include <stdlib.h>

const int PCB::NEW=0;
const int PCB::READY=1;
const int PCB::BLOCKED=2;
const int PCB::OVER=3;

PCB::PCB(Thread* myT, StackSize stSize, Time tmSlc){
    lock();
    myThread=myT;
    state=NEW;
    if(stSize>64*1024) stSize=64*1024;
    stackSize=stSize;
    timeSlice=tmSlc;
    timeElapsed=0;
    stack=NULL;
    unlock();
}

PCB::~PCB(){
    lock();
    delete stack;
    unlock();
}

void PCB::wrapper(){
    Thread::running->run();
    lock();
    Thread::running->myPCB->state=OVER;
    Thread* temp;
    while(1){
        temp=Thread::running->waitingThreads->get();
        if(!temp) break;
        temp->myPCB->state=READY;
        Scheduler::put(temp->myPCB);
    }
    unlock();
    dispatch();
}

void PCB::initStack(){
    lock();
    int n=stackSize/2;
    stack=new unsigned[n];

    stack[n-1]=0x200;
    stack[n-2]=FP_SEG(&(PCB::wrapper));
    stack[n-3]=FP_OFF(&(PCB::wrapper));

    ss=FP_SEG(stack+n-12);
    sp=FP_OFF(stack+n-12);
    unlock();
}