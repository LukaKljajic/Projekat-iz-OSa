#include "ksemlist.h"

class KSemList;

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
