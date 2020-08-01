#include "queue.h"

Queue::Queue(){
    lock();
    last=first=NULL;
    size=0;
    unlock();
}

Queue::~Queue(){
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

void Queue::put(Thread* t){
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

Thread* Queue::get(){
    lock();
    if(first==NULL){
        unlock();
        return NULL;
    }
    Elem* old=first;
    Thread* res=old->info;
    first=first->next;
    if (first==NULL) last=NULL;
    delete old;
    size--;
    unlock();
    return res;
}

int Queue::getSize(){
    return size;
}

Thread* Queue::findByID(ID id){
    Elem* temp=first;
    for(; temp && temp->info->getId()!=id; temp=temp->next);
    return temp->info;
}