#ifndef _KERNELEV_H_
#define _KERNELEV_H_

#include "event.h"
#include "ivtentry.h"
#include "kernelse.h"

typedef unsigned char IVTNo;

class KernelEv{
public:
    KernelEv(IVTNo ivtNo);
    ~KernelEv();
    void wait();
    void signal();
private:
    IVTNo ivtNo;
    KernelSem* kSem;
};

#endif