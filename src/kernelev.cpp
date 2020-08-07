#include "kernelev.h"

KernelEv::KernelEv(IVTNo ivtNo){
    this->ivtNo=ivtNo;
    kSem=new KernelSem(0);
    IVTEntry::getEntry(ivtNo)->setKEvent(this);
}

KernelEv::~KernelEv(){
    IVTEntry::getEntry(ivtNo)->setKEvent(NULL);
}

void KernelEv::wait(){
    kSem->wait(0);
}

void KernelEv::signal(){
    if(kSem->val()!=1) kSem->signal(0);
}