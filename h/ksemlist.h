#ifndef _KSEMLIST_H_
#define _KSEMLIST_H_

#include "kernelse.h"
#include "global.h"

class KernelSem;

class KSemList{
private:
    friend class KernelSem;
    struct Elem{
        KernelSem* info;
        Elem* next;
        Elem(KernelSem* i, Elem* n=NULL):info(i), next(n){}
    };
    Elem* first, *last, *current;
    int size;
    static KSemList semaphores;
    KSemList();
    void put(KernelSem* t);
    KernelSem* get();
    int getSize();
public:
    virtual ~KSemList();
    static void deblockByTime();
};

#endif