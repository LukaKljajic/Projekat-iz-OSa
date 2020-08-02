#include "mainthr.h"
#include "thread.h"

volatile Thread* MainThread::main=NULL;

MainThread::MainThread():Thread(0x5000, 1){}

Thread* MainThread::getMain(){
    if(!main) main=new MainThread;
    return (Thread*)main;
}

void MainThread::deleteMain(){
    delete main;
    //main=NULL;
}