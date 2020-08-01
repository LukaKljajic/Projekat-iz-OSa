#include "global.h"
#include "thread.h"
#include "idle.h"
#include "schedule.h"
#include <dos.h>
#include <stdlib.h>
#include <iostream.h>

volatile int Global::lockFlag=0;
volatile int Global::contextSwitchOnDemand=0;
void interrupt (*Global::oldTimerInterrupt)(...)=NULL;

void Global::initialize(){
    lock();
    cout<<"Poceo sam inicijalizaciju"<<endl;
    Thread::running=Thread::mainThread=new Thread(0x5000, 1);
    Thread::mainThread->myPCB->state=PCB::READY;
    oldTimerInterrupt=getvect(0x08);
    setvect(0x08, timerInterrupt);
    IdleThread* idle=IdleThread::getIdle();
    idle->start();
    cout<<"inicijalizovao sam"<<endl;
    unlock();
}

void Global::finalize(){
    if(Thread::running!=Thread::mainThread) return;
    lock();
    cout<<"Usao u finalize"<<endl;
    setvect(0x08, oldTimerInterrupt);
    unlock();
    delete Thread::mainThread;
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
    }

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
                    // cout<<"Vrh steka je SS "<<FP_SEG(Thread::running->myPCB->stack+Thread::running->myPCB->stackSize/2-1)<<", SP "<<FP_OFF(Thread::running->myPCB->stack+Thread::running->myPCB->stackSize/2-1)<<endl;
                    // cout<<"Trenutni SS je "<<tss<<", SP "<<tsp<<endl;
                    break;
                }
            }
        }
        else contextSwitchOnDemand=1;
        Thread::running->myPCB->timeElapsed=0;
    }

    if(!contextSwitchOnDemand){
        //tick(); //otkomentarisati kad se poveze sa testovima
        (*oldTimerInterrupt)();
    }

    // cout<<"Izasao iz prekidne rutine"<<endl;
    // cout<<flush;
}