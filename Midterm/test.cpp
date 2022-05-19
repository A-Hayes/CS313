#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;


int main() {
	for (int i=0; i<3; i++){
	  //cout << "1PID = "<< getpid() << ", i = " << i << endl;
    int f = fork();
    //wait(0);
    /*
    cout << "past fork " << getpid() << endl;
    if (i == 0 || i == 2){
      wait(0);
      cout << "waited for child in " << getpid()  << endl;
    }
    */
	  cout << "PID = " << getpid() << ", i = " << i << endl;
  }
}  
