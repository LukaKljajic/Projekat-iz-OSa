#include "global.h"
#include "thread.h"
#include "idle.h"
#include "schedule.h"
#include <dos.h>
#include <stdlib.h>

volatile int Global::lockFlag=0;
volatile int Global::contextSwitchOnDemand=0;
void interrupt (*Global::oldTimerInterrupt)(...)=NULL;

void Global::initialize(){
    lock();
    Thread::running=Thread::mainThread=new Thread(0x5000, 1);
    Thread::mainThread->myPCB->state=PCB::READY;
    oldTimerInterrupt=getvect(0x08);
    setvect(0x08, timerInterrupt);
    IdleThread* idle=IdleThread::getIdle();
    idle->start();
    unlock();
}

void Global::finalize(){
    if(Thread::running!=Thread::mainThread) return;
    lock();
    setvect(0x08, oldTimerInterrupt);
    delete Thread::mainThread;
    IdleThread::deleteIdle();
    unlock();
}

void Global::dispatch(){
    lock();
    contextSwitchOnDemand=1;
    timerInterrupt();
    contextSwitchOnDemand=0;
    unlock();
}

void interrupt Global::timerInterrupt(...){
    static volatile unsigned tss, tsp;

    if(!contextSwitchOnDemand && Thread::running->myPCB->timeSlice!=0){
        Thread::running->myPCB->timeElapsed++;
    }

    if(contextSwitchOnDemand || (Thread::running->myPCB->timeElapsed==Thread::running->myPCB->timeSlice && Thread::running->myPCB->timeSlice!=0 && lockFlag==0)){
        if(Thread::running->myPCB->state==PCB::READY && Thread::running!=IdleThread::getIdle()){
            Scheduler::put(Thread::running->myPCB);
        }
        volatile PCB* newPCB;
        while(1){
            newPCB=Scheduler::get();
            if(!newPCB) newPCB=IdleThread::getIdle()->myPCB;
            if(newPCB->state==PCB::READY){
                asm {
                    mov tsp, sp
                    mov tss, ss
                }
                Thread::running->myPCB->sp = tsp;
                Thread::running->myPCB->ss = tss;
                Thread::running=newPCB->myThread;
                tsp = Thread::running->myPCB->sp;
                tss = Thread::running->myPCB->ss; 
                asm {
                    mov sp, tsp
                    mov ss, tss
                }
                break;
            }
        }
        Thread::running->myPCB->timeElapsed=0;
    }

    if(contextSwitchOnDemand) return;
    //tick(); //otkomentarisati kad se poveze sa testovima
    (*oldTimerInterrupt)();
}