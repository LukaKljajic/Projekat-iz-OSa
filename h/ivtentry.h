#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

#include "event.h"
#include "kernelev.h"

typedef unsigned char IVTNo;
typedef void interrupt (*Interrupt)(...);
class KernelEv;

#define PREPAREENTRY(No, flag)\
    void interrupt intr##No(...);\
    IVTEntry ivte##No(No, intr##No);\
    void interrupt intr##No(...){\
        if(flag) ivte##No.callOldRoutine();\
        ivte##No.signal();\
    }


class IVTEntry{
public:
    IVTEntry(IVTNo ivtNo, Interrupt intr);
    virtual ~IVTEntry();
    void callOldRoutine();
    void signal();
    void setKEvent(KernelEv*);
    static IVTEntry* getEntry(IVTNo ivtNo);
private:
    friend class KernelEv;
    static IVTEntry* entries[];
    KernelEv* kEvent;
    void interrupt(*oldRoutine)(...);
    IVTNo ivtNo;
};

#endif