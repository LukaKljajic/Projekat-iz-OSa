#include "global.h"
#include "thread.h"
#include "idle.h"
#include "schedule.h"
#include "mainthr.h"
#include "kernelse.h"
#include "ksemlist.h"
#include <dos.h>
#include <stdlib.h>
#include <iostream.h>

volatile int Global::lockFlag=0;
volatile int Global::contextSwitchOnDemand=0;
void interrupt (*Global::oldTimerInterrupt)(...)=NULL;

void Global::initialize(){
    lock();
    Thread::running=MainThread::getMain();
    Thread::running->myPCB->state=PCB::READY;
    oldTimerInterrupt=getvect(0x08);
    setvect(0x08, timerInterrupt);
    Thread* idle=IdleThread::getIdle();
    idle->start();
    unlock();
}

void Global::finalize(){
    if(Thread::running!=MainThread::getMain()) return;
    lock();
    printDebug("Usao u finalize");
    setvect(0x08, oldTimerInterrupt);
    Thread::running=NULL;
    unlock();
    MainThread::deleteMain();
    IdleThread::deleteIdle();
}

void Global::dispatch(){
    contextSwitchOnDemand=1;
    timerInterrupt();
}

void interrupt Global::timerInterrupt(...){
    asm cli;
    // cout<<"Usao u prekidnu rutinu"<<endl;
    // if(contextSwitchOnDemand) cout<<"Zahtevana promena konteksta"<<endl;
    // if(lockFlag) cout<<"Zabranjeno preuzimanje: "<<lockFlag<<endl;
    // else cout<<"Dozvoljeno preuzimanje"<<endl;

    static volatile unsigned tss, tsp;
    static volatile PCB* newPCB;

    if(!contextSwitchOnDemand && Thread::running->myPCB->timeSlice!=0){
        Thread::running->myPCB->timeElapsed++;
        // cout<<"Proslo je "<<Thread::running->myPCB->timeElapsed<<" vremena"<<endl;
    }
    // else cout<<"Ova nit nema zadato vreme"<<endl;

    if((contextSwitchOnDemand || (Thread::running->myPCB->timeElapsed==Thread::running->myPCB->timeSlice && Thread::running->myPCB->timeSlice!=0))){
        if(lockFlag==0){
            contextSwitchOnDemand=0;
            if(Thread::running->myPCB->state==PCB::READY && Thread::running!=IdleThread::getIdle()){
                Scheduler::put(Thread::running->myPCB);
            }
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
                    // cout<<"Izabrana je nit sa ID: "<<Thread::getRunningId()<<endl;
                    // if(Thread::running==IdleThread::getIdle()) cout<<"izabrana je idle nit"<<endl;
                    break;
                }
            }
        }
        else contextSwitchOnDemand=1;
        Thread::running->myPCB->timeElapsed=0;
    }

    if(!contextSwitchOnDemand){
        tick(); //otkomentarisati kad se poveze sa testovima
        (*oldTimerInterrupt)();
        KSemList::deblockByTime();
    }

    // cout<<"Izasao iz prekidne rutine"<<endl;
}