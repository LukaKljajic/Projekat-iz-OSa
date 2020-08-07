#include "thread.h"
#include "semaphor.h"
#include "event.h"
#include "global.h"
#include <iostream.h>
#include <dos.h>

class Thread;

Semaphore printSem(1);
#define print(X) {printSem.wait(0); cout << X << endl; printSem.signal(0);}

const int BUFFER_SIZE = 20;
int end=0;

class BoundedBuffer {
public:
   BoundedBuffer ();
   void append (char);
   void appendChunk(char*, int);
   char take ();
   int  size() {return itemAvailable.val();}; 
private:
   Semaphore mutex;
   Semaphore spaceAvailable, itemAvailable;
   char buffer[BUFFER_SIZE];
   int head, tail;
};

BoundedBuffer::BoundedBuffer () : mutex(1), spaceAvailable(BUFFER_SIZE), itemAvailable(0),
   head(0), tail(0) {}

void BoundedBuffer::append (char d) {
   spaceAvailable.wait(0);
   mutex.wait(0);
   buffer[tail] = d;
   tail = (tail+1)%BUFFER_SIZE;
   mutex.signal(0);
   itemAvailable.signal(0);
}

void BoundedBuffer::appendChunk(char* chunk, int size){
   for(int i=0;i<size;i++){
      spaceAvailable.wait(0);
      mutex.wait(0);
      buffer[tail] = chunk[i];
      tail = (tail+1)%BUFFER_SIZE;
      mutex.signal(0);
   }
   itemAvailable.signal(size);
}

char BoundedBuffer::take () {
   itemAvailable.wait(0);
   mutex.wait(0);
   char d = buffer[head];
   head = (head+1)%BUFFER_SIZE;
   mutex.signal(0);
   spaceAvailable.signal(0);
   return d;
}

void outputDelay() {
   delay(20);
};

class Producer : public Thread {
public:
   Producer(BoundedBuffer* bb,char y):myBuffer(bb),x(y) {}
protected:
   virtual void run ();
   char produce();
   BoundedBuffer* myBuffer;
   char x;
};

char Producer::produce() {
   return x;
}; // Produce an item

void Producer::run () {
   while(!end) {
      outputDelay();
      char d = produce();
      myBuffer->append(d);
      print("proizveo "<<x<<" u baferu ima "<<myBuffer->size()<<" elemenata");
   }
}

class MassProducer : public Producer{
public:
   MassProducer(BoundedBuffer* bb,char y):Producer(bb,y){}
protected:
   virtual void run();
};

void MassProducer::run(){
   while(!end){
      outputDelay();
      char d[10];
      for(int i=0;i<10;i++) d[i] = produce();
      myBuffer->appendChunk(d, 10);
      print("proizveo 10 "<<x<<" u baferu ima "<<myBuffer->size()<<" elemenata");
   }
}

class Consumer : public Thread {
public:
   Consumer (BoundedBuffer* bb) : myBuffer(bb) {}
protected:
   virtual void run ();
   void consume(char p);
private:
   BoundedBuffer* myBuffer;
};

void Consumer::consume(char p){print(p);} // Consume an item

void Consumer::run () {
   while(!end) {
      outputDelay();
      char d = myBuffer->take();
      consume(d);
   }
  
   while (myBuffer->size()){
      outputDelay();
      char d = myBuffer->take();
      consume(d);
   }
   print("\nKraj PRODUCER-BUFFER-CONSUMER!");
}

class PrintingThread : public Thread {
public:
	PrintingThread(char *nm, StackSize sts = defaultStackSize, Time tslc = defaultTimeSlice) : Thread(sts, tslc), name(nm) {};
protected:
	virtual void run();	
private:
	char *name;
};

void PrintingThread::run() {
	long i,j;
	for (i=0;i<300;i++) {
		print(name << " : rUnNiNg!");
		for (j=0;j<1000000;j++);
	}
}

class MinimalThread : public Thread {
public:
	MinimalThread() : Thread(defaultStackSize, 1), number(0) {};
	void setNumber(int n) {number = n;};
protected:
	virtual void run();
private:
	int number;
};

void MinimalThread::run() {
	print("MinimalThread - " << number << ":eNdInG!");
}

PREPAREENTRY(9, 1);
Event event(9);

class KeyboardEvent : public Thread{
public:
   KeyboardEvent(StackSize ss=defaultStackSize,Time slc=defaultTimeSlice):Thread(ss,slc) {};
protected:
   virtual void run();
};

void KeyboardEvent::run(){
   print("KeYbOaRd EvEnT: wAiTiNg!");
	event.wait();
	print("KeYbOaRd EvEnT!");
   end=1;
}

int userMain(int argc, char* argv[]) {
   long i,j;

   BoundedBuffer buff;
   Consumer con(&buff);
   Producer * pro[4];

   for (i=0; i<3; i++){
      pro[i] = new Producer(&buff, '0'+i);
      pro[i]->start();
   } 
   pro[3] = new MassProducer(&buff, '0'+3);
   pro[3]->start();
   con.start();

   PrintingThread t1("Thread 1", defaultStackSize, 1),t2("Thread 2", defaultStackSize, 3),t3("Thread 3", defaultStackSize, 2);								 
	t1.start();
	t2.start();
	t3.start();
	
	MinimalThread threads[20];
	for (i=0;i<20;i++) threads[i].setNumber(i);
	for (i=0;i<20;i++) threads[i].start();

   KeyboardEvent kbdEvent;
   kbdEvent.start();

   for(i=1;i<=20;i++) {
      print("userMain(" << i << ")");
      delay(3);
   }

   for(i=0;i<4;i++){
      pro[i]->waitToComplete();
      delete pro[i];
   }

   return 0;
}
