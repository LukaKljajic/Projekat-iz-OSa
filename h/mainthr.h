#ifndef _MAINTHR_H_
#define _MAINTHR_H_

#include "thread.h"

class Thread;

class MainThread:public Thread{
    MainThread();
    static volatile Thread* main;
public:
    static Thread* getMain();
    static void deleteMain();
};

#endif