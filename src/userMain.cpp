#include "global.h"
#include "queue.h"
#include "thread.h"
#include <iostream.h>

class Thread;
class Trkac:public Thread{
public:
    Trkac(int m):Thread(), metara(m){}
protected:
    virtual void run();
private: 
    int metara;
};

void Trkac::run(){
    while(metara>0){
        lock();
        cout<<"Trkacu broj "<<getId()<<" ostalo je jos "<<--metara<<" metara"<<endl;
        unlock();
        for(int i=0; i<10000; i++)
            for(int j=0; j<10000; j++);
    }
}

int userMain(int argc, char* argv[]){
    Thread* niti[2];
    int i;
    for(i=0;i<2;i++){
        niti[i]=new Trkac(i+50);
        lock();
        cout<<"stvorio nit "<<niti[i]->getId()<<endl;
        unlock();
        niti[i]->start();
        lock();
        cout<<"pokrenuo nit "<<niti[i]->getId()<<endl;
        unlock();
    }
    while(1);
    for(i=0;i<2;i++){
        delete niti[i];
    }
    return 0;
}