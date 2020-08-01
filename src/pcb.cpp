#include "pcb.h"
#include "thread.h"
#include "queue.h"
#include "schedule.h"
#include "idle.h"
#include <dos.h>
#include <stdlib.h>
#include <iostream.h>

const int PCB::NEW=0;
const int PCB::READY=1;
const int PCB::BLOCKED=2;
const int PCB::OVER=3;

PCB::PCB(Thread* myT, StackSize stSize, Time tmSlc){
    lock();
    myThread=myT;
    state=NEW;
    if(stSize>65536) stackSize=65536;
    else stackSize=stSize;
    cout<<"pcbstsize "<<stackSize<<endl;
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
    lock();
    cout<<"usao u nit sa ID: "<<Thread::running->getId()<<endl;
    if(Thread::running==IdleThread::getIdle()) cout<<"Ovo je idle nit"<<endl;
    unlock();

    Thread::running->run();

    lock();
    cout<<flush;
    cout<<"Zavrsio nit"<<endl;
    Thread::running->myPCB->state=OVER;
    Thread* temp;
    while(1){
        temp=Thread::running->waitingThreads->get();
        if(!temp) break;
        temp->myPCB->state=READY;
        Scheduler::put(temp->myPCB);
        cout<<"Izbacio nit "<<temp->getId()<<" iz reda cekanja"<<endl;
    }
    cout<<Global::lockFlag<<endl;
    unlock();
    dispatch();
}

void PCB::initStack(){
    lock();
    cout<<"velicina steka je "<<stackSize<<endl;
    if(myThread==IdleThread::getIdle()) cout<<"ovo je idle nit"<<endl;
    static int n=stackSize/2;
    stack=new unsigned[n];

    stack[n-1]=0x200;
    stack[n-2]=FP_SEG(&(PCB::wrapper));
    stack[n-3]=FP_OFF(&(PCB::wrapper));

    ss=FP_SEG(stack+n-12);
    sp=FP_OFF(stack+n-12);
    cout<<FP_SEG(stack+n-1)<<" "<<FP_OFF(stack+n-1)<<endl;
    cout<<"ss="<<ss<<", sp="<<sp<<endl;
    cout<<"stvorio stek niti sa ID: "<<myThread->getId()<<endl;
    unlock();
}

// volatile unsigned newss, newsp, newcs, newip, oldss, oldsp;

// void PCB::initStack(){
//     lock();
//     cout<<"velicina steka je "<<stackSize<<endl;
//     stack=new unsigned char[stackSize];
//     newss=FP_SEG(stack+stackSize-1);
//     newsp=FP_OFF(stack+stackSize-1);
//     newcs=FP_SEG(&(PCB::wrapper));
//     newip=FP_OFF(&(PCB::wrapper));
//     asm{
//         mov oldss, ss
//         mov oldsp, sp
//         mov ss, newss
//         mov sp, newsp

//         pushf
//         pop ax
//         or ax, 0000001000000000b
//         push ax

//         mov ax, newcs
//         push ax
//         mov ax, newip
//         push ax

//         mov ax, 0
// 		push ax
// 		push bx
// 		push cx
// 		push dx
// 		push es
// 		push ds
// 		push si
// 		push di
//         mov ax, 0
// 		push ax

//         mov newss, ss
//         mov newsp, sp

//         mov ss, oldss
//         mov sp, oldsp
//     }
//     ss=newss;
//     sp=newsp;
//     // cout<<FP_SEG(stack+stackSize-1)<<" "<<FP_OFF(stack+stackSize-1)<<endl;
//     // cout<<"ss="<<ss<<", sp="<<sp<<endl;
//     // cout<<"stvorio stek niti sa ID: "<<myThread->getId()<<endl;
//     unlock();
// }