#include <iostream>
#include <thread>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include "Semaphore.h"
using namespace std;

/* This program now only works for NP=1 (i.e., 1 producer and NC consumers). 
 Modify your program such that when you change NP to >1,
 exactly NP producers run first, then exactly NC, and the whole thing
 should repeat in that order */

#define NP 8     
#define NC 5

int buffer = 0;
Semaphore consumerdone (1);
Semaphore producerdone (0);
Semaphore mtx (1); // will use as mutex
int ncdone = 0; // number of consumers done consuming
int npdone = 0; // number of producers done producing

// each producer gets an id, which is pno	
void producer_function (int pno){
	while (true){
		// do necessary wait
		consumerdone.P();		// wait for the buffer to be empty

		// after wait is done, do the produce operation
		// you should not need to change this block
		mtx.P();
		buffer ++;
		cout << "Producer [" << pno << "] left buffer=" << buffer << endl;
		mtx.V();
    usleep (500000);

		// now do whatever that would indicate that the producers are done
		// in this case, the single producer is waking up all NC consumers
		// this will have to change when you have NP producers
		
		mtx.P();
		npdone ++;
		if (npdone == NP){ // it is the last one 
		  //incorporates orginally given loop to start proper amount of new consumers
			for (int i=0; i<NC; i++)
			  producerdone.V(); // so wake up the producer
			npdone = 0; // reset the counter
	  //contingency for start of program, makes sure enough producer threads are
	  //started to get the pattern running properly. 
		}else if(buffer < NP){
		  consumerdone.V();
		}
		mtx.V();
		
		//for (int i=0; i<NC; i++)
		//	producerdone.V();
	}
}
// each consumer gets an id cno
void consumer_function (int cno){
	while (true){
		//do necessary wait
		producerdone.P();
		// each consumer reads the buffer		
		// you should not need to change this block
		mtx.P();
		cout << ">>>>>>>>>>>>>>>>>>>>Consumer [" <<cno<<"] got <<<<<<<<<<" << buffer << endl;
		mtx.V();
		usleep (500000);

		// now do whatever necessary that would indicate that the consumers are all done
		mtx.P();
		ncdone ++;
		if (ncdone == NC){ // it is the last one 
		  //added similar forloop to ensure proper amount of producers are unlocked
			for (int i=0; i<NP; i++)
			  consumerdone.V(); // so wake up the producer
			ncdone = 0; // reset the counter
		}
		mtx.V();
	}
}


int main (){
	vector<thread> producers;
	vector<thread> consumers;

	for (int i=0; i< 100; i++)
		producers.push_back (thread (producer_function, i+1));

	for (int i=0; i< 300; i++)
		consumers.push_back (thread (consumer_function, i+1));

	
	for (int i=0; i<consumers.size (); i++)
		consumers [i].join();
	
	for (int i=0; i<producers.size (); i++)
		producers [i].join();
	
}

