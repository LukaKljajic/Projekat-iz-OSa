#include "ivtentry.h"

IVTEntry* IVTEntry::entries[256];

IVTEntry::IVTEntry(IVTNo ivtNo, Interrupt intr){
    kEvent=NULL;
    this->ivtNo=ivtNo;
    entries[ivtNo]=this;
    lock();
    oldRoutine=getvect(ivtNo);
    setvect(ivtNo, intr);
    unlock();
}

IVTEntry::~IVTEntry(){
    lock();
    setvect(ivtNo, oldRoutine);
    unlock();
}

void IVTEntry::callOldRoutine(){
    oldRoutine();
}

void IVTEntry::signal(){
    kEvent->signal();
}

void IVTEntry::setKEvent(KernelEv* kEv){
    kEvent=kEv;
}

IVTEntry* IVTEntry::getEntry(IVTNo ivtNo){
    return entries[ivtNo];
}