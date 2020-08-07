#include "thread.h"
#include "semaphor.h"
#include <iostream.h>
#include <dos.h>

#define print(X) {printSem.wait(0); cout<<X<<endl; printSem.signal(0); }

class Thread;

Semaphore s1(1), s2(4), mutex(1), printSem(1);
int number = 1;

class MyThread:public Thread{
public:
    MyThread(int hm, int op, int sn):howMuch(hm), operation(op), secondNum(sn){}
protected:
    virtual void run();
private:
    int howMuch;
    int operation;
    int secondNum;
};

void MyThread::run(){
    while(howMuch--){
        delay(20);
        s1.wait(4);
        s2.wait(5);
        mutex.wait(0);
        switch(operation){
            case 0:
                print("Sabiranje sa "<<secondNum);
                number+=secondNum;
                break;
            case 1:
                print("Oduzimanje sa "<<secondNum);
                number-=secondNum;
                break;
            case 2:
                print("Mnozenje sa "<<secondNum);
                number*=secondNum;
                break;
            case 3:
                print("Deljenje sa "<<secondNum);
                number/=secondNum;
                break;
        }
        mutex.signal(0);
        print("Broj je sada "<<number);
    }
}

int userMain(int argc, char* argv[]) {
    int i;
    MyThread* threads[5];

    for(i=0;i<5;i++){
        threads[i]=new MyThread(10, i%4, 2+i);
        threads[i]->start();
    }

    for(i=1;i<=20;i++) {
        print("userMain(" << i << ")");
        delay(5);
    }

    s1.signal(4);
    s2.signal(7);

    for(i=0; i<5;i++){
        threads[i]->waitToComplete();
        delete threads[i];
    }

    return 0;
}