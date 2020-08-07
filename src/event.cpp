#include "event.h"

Event::Event(IVTNo ivtNo){
    lock();
    myImpl=new KernelEv(ivtNo);
    creator=(Thread*)Thread::running;
    unlock();
}

Event::~Event(){
    lock();
    delete myImpl;
    unlock();
}

void Event::wait(){
    if(creator!=Thread::running) return;
    lock();
    myImpl->wait();
    unlock();
}

void Event::signal(){
    lock();
    myImpl->signal();
    unlock();
}