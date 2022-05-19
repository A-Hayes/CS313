#include <iostream>
#include <thread>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include "Semaphore.h"
using namespace std;

//create condition semaphores to control execution sequence
//c is initialized at 1, as A relies on C to start proper order
Semaphore aIsDone (0);
Semaphore b1IsDone (0);
Semaphore b2IsDone (0);
Semaphore cIsDone (1);
Semaphore mtx (0);  //general utility mutex
int stage = 0; //keeps track of stage of sequence for b1 and b2; a=1, b1=2, b2=3

void A_function (){
  while(true){
    cIsDone.P();
    
    mtx.P();
    stage = 1;
    cout << "A ";
    mtx.V();
    
    aIsDone.V();
  }
}

void B_function (){
  while(true){
    //elseif condition not technically necessary, mainly for demonstration
    //uses stage to decide which signal to wait for
    if(stage == 1){
      aIsDone.P();
    }else if(stage == 2){
      b1IsDone.P();
    }
    
    mtx.P();
    cout << "B ";
    stage++;
    mtx.V();
    
    //uses stage to decide which signal to send
    if(stage == 2){
      b1IsDone.V();
    }else if(stage == 3){
      b2IsDone.v();
    }
  }
}

void C_function (){
  while(true){
    b2IsDone.P();
    
    mtx.P();
    cout << "C" << endl;
    mtx.V();
    
    cIsDone.V();
  }
}


int main (){
  vector<thread> threads;
  
  threads.push_back( thread(A_function) );
  threads.push_back( thread(B_function) );
  threads.push_back( thread(B_function) );
  threads.push_back( thread(C_function) );
  
  for (int i=0; i<threads.size (); i++)
		threads[i].join();
  
  
}
