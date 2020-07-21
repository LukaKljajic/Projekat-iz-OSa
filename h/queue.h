#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "global.h"
#include "thread.h"
#include <dos.h>
#include <stdlib.h>

class Global;
class Thread;
typedef int ID;

class Queue{
private:
    struct Elem{
        Thread* info;
        Elem* next;
        Elem(Thread* i, Elem* n=NULL):info(i), next(n){}
    };
    Elem* first, *last;
    int size;
public:
    Queue();
    virtual ~Queue();
    void put(Thread* t);
    Thread* get();
    int getSize();
    Thread* findByID(ID id);
};

#endif