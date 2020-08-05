#include "ksemlist.h"
#include "schedule.h"
#include "global.h"

class KSemList;

KSemList KSemList::semaphores;

KSemList::KSemList(){
    lock();
    current=last=first=NULL;
    size=0;
    unlock();
}

KSemList::~KSemList(){
    lock();
    Elem* temp=first, *old;
    while(temp){
        old=temp;
        temp=temp->next;
        delete old;
    }
    size=0;
    unlock();
}

void KSemList::put(KernelSem* t){
    lock();
    Elem* current;
    for(current=first; current != NULL; current=current->next){
        if(current->info == t){
            unlock();
            return;
        }
    }
    Elem* newbie=new Elem(t);
    if (first==NULL){
        first=last=newbie;
    }
    else{
        last->next=newbie;
        last=last->next;
    }
    size++;
    unlock();
}

KernelSem* KSemList::get(){
    lock();
    if(first==NULL){
        unlock();
        return NULL;
    }
    Elem* old=first;
    KernelSem* res=old->info;
    first=first->next;
    if (first==NULL) last=NULL;
    delete old;
    size--;
    unlock();
    return res;
}

int KSemList::getSize(){
    return size;
}

void KSemList::deblockByTime(){
    for(semaphores.current=semaphores.first; semaphores.current; semaphores.current=semaphores.current->next){
        // printDebug("u petlji sam ");
        KernelSem* semaphore=semaphores.current->info;
        Queue* threads=semaphore->waitingThreads;
        threads->prev=NULL;
        threads->current=threads->first;
        while(threads->current){
            Thread* thread=threads->current->info;
            if(thread->myPCB->waitingTime>0 && --thread->myPCB->waitingTime==0){
                printDebug("Odblokirace nit "<<thread->getId()<<" prekidom");
                Queue::Elem* old;
                if(threads->prev){
                    threads->prev->next=threads->current->next;
                    old=threads->current;
                    threads->current=threads->current->next;
                }
                else{
                    old=threads->current;
                    threads->first=threads->current=threads->current->next;
                    if(!threads->first) threads->last=NULL;
                }
                semaphore->value++;
                thread->myPCB->unblockedByTime=1;
                thread->myPCB->state=PCB::READY;
                Scheduler::put(thread->myPCB);
                delete old;
            }
            else{
                threads->prev=threads->current;
                threads->current=threads->current->next;
            }
        }
        // printDebug("izasao iz ugnezdene petlje");
    }
    // printDebug("izasao iz petlje");
}
