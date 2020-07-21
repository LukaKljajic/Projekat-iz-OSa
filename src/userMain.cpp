#include "global.h"
#include "queue.h"
#include "thread.h"
#include <iostream.h>

class Thread;
class Trkac:public Thread{
public:
    Trkac(int m):Thread(), metara(m){}
protected:
    virtual void run(){
        while(metara>0){
            lock();
            cout<<"Trkacu broj "<<getId()<<" ostalo je jos "<<--metara<<" metara"<<endl;
            unlock();
        }
    }
private: 
    int metara;
};

int userMain(int argc, char* argv[]){
    Thread* niti[4];
    int i;
    for(i=0;i<4;i++){
        niti[i]=new Trkac(i+50);
        niti[i]->start();
    }
    for(i=0;i<4;i++){
        delete niti[i];
    }
    return 0;
}