/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include <fstream>
#include <iostream>
#include <time.h>
#include "common.h"
#include "FIFOreqchannel.h"

using namespace std;


int main(int argc, char *argv[]){
    //
    //INIT BLOCK
    //
    //use fork and exec to start server as a fork of client
    //make sure to use some sleep() statements to ensure sever has time to respond to client requests
    
    int opt, person, ecg;
    int maxBuf = MAX_MESSAGE;
    double inTime = 0;
    int makeChannel = 0, getFile = 0;
    double data = 0;
    string filename = "";
    
    int debug;
    
    
    //getopt structure to handle argument cases
    //if no arguments, program opens server as fork, creates control channel, and closes then exits
    while ((opt = getopt(argc, argv, "p:e:t:m:f:c")) != -1) {
	  switch (opt) {
	    case 'p':
	      person = atoi(optarg);
	      break;
	    case 't':
	      inTime = atof(optarg);
	      break;
	    case 'e':
	      ecg = atoi(optarg);
	      break;
	    case 'm':
	      maxBuf = atoi(optarg);
	      break;
	    case 'f':
	      filename = optarg;
	      getFile = 1;
	      break;
	    case 'c':
	      makeChannel = 1;
	      break;
	  }
	}
	
	//note: server file getopt structure edited to suppress error messages for arguments besides -m
	int pid;
    pid = fork();
    if(pid == 0){
      execvp("./server", argv);
      sleep(3);
    }
    
    //create main control channel
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
    
    
    
    
    
    //
    //DATAMSG BLOCK
    //
    if(person > 0 && inTime >= 0 && ecg > 0 && ecg < 3){
    //create new datamsg with parameters for target datapoint
      datamsg *d1 = new datamsg(person, inTime, ecg);
      //send request to server
      chan.cwrite(d1, sizeof(datamsg) );
      //allocate variable as space and write server response to said variable; print; delete for cleanup
      chan.cread(&data, sizeof(double) );
      printf("data = %lf \n", data);
      delete d1;
    }
    
    
    
    
    
    /*
    //1K DATA POINTS BLOCK
    //
    //This section removed for normal function, x1.csv generated into /received and checked
    //repeat above process to fill a file (x1.csv) with 1000 data points
    
    //time measurment setup, take first/before timecheck
    struct timeval start1k, end1k; 
    gettimeofday(&start1k, NULL); 
    
    //create and open file and fstream to read file
    ofstream x1;
    x1.open("received/x1.csv");
    double time = 0;
    
    //writes first 1k data points of person 1 to x1.csv in same format as source file. 
    for(int i=0; i<1000; i++){
      time = i*0.004;
      datamsg *da = new datamsg(1, time, 1);
      chan.cwrite(da, sizeof(datamsg) );
      data = 0;
      chan.cread(&data, sizeof(double) );
      x1 << time << "," << data << ","; 
      delete da;
      datamsg *db = new datamsg(1, time, 2);
      chan.cwrite(db, sizeof(datamsg) );
      data = 0;
      chan.cread(&data, sizeof(double) );
      x1 << data << "\n"; 
      delete db;
    }
    x1.close();
    
    //take last/end timecheck
    gettimeofday(&end1k, NULL); 
    
    //timing calc
    double totalTime; 
    totalTime = (end1k.tv_sec - start1k.tv_sec) * 1e6; 
    totalTime = (totalTime + (end1k.tv_usec - start1k.tv_usec)) * 1e-6; 
    cout << "Transfer of 1k data points takes [" << totalTime << setprecision(6) << "] seconds." << endl;
    */
    
    
    
    
    
    //
    //FILEMSG BLOCK
    //
    if(getFile){
      //timing block
      //time measurment setup, take first/before timecheck
      struct timeval startFile, endFile; 
      gettimeofday(&startFile, NULL); 
      
      //create filemsg to request file "filename"
      filemsg file (0, 0);
      //creat buffer for size of filemsg and filename
      int request = sizeof(filemsg) + filename.size() + 1;
      char* buf = new char[request];
      //load the filemsg into buffer, then the filename, then send request to server
      memcpy(buf, &file, sizeof(filemsg) );
      memcpy(buf + sizeof(filemsg), filename.c_str(), filename.size() + 1);
      chan.cwrite(buf, request);
      
      //read and save file length from sever response 
      __int64_t filesize;
      chan.cread(&filesize, sizeof(__int64_t) );
      cout << "Filelength: " << filesize << endl;
      __int64_t dispSize = filesize;
    
      //create empty file with same name as source to allocate source file's data to
      string outPath = string("received/") + filename;
      FILE *f = fopen(outPath.c_str(), "wb");
      //create receiver buffer to read in data from source file over server
      //size of each 'chunk' is MAX_MESSAGE, to handle the server's max allowable transfer
      char *receiver = new char[maxBuf];
    
      //until entire filesize is iterated over, take chunks into receiver buffer and write to new file
      while(filesize>0){
        int rLength = min( (__int64_t)maxBuf, filesize);
        ((filemsg*)buf)->length = rLength;
        chan.cwrite(buf, request);
        chan.cread(receiver, rLength);
        fwrite(receiver, 1, rLength, f);
        ((filemsg*)buf)->offset += rLength;
        filesize -= rLength;
      }
    
      //finish adding to file; buffer cleanup
      fclose(f);
      delete buf;
      delete receiver;
      
      //take last/end timecheck
      gettimeofday(&endFile, NULL); 
      
      //timing calc
      double totalTime_file; 
      totalTime_file = (endFile.tv_sec - startFile.tv_sec) * 1e6; 
      totalTime_file = (totalTime_file + (endFile.tv_usec - startFile.tv_usec)) * 1e-6; 
      cout << "Transfer of " << dispSize << "-byte file takes [" << totalTime_file << setprecision(6);
      cout << "] seconds." << endl;
    }
    
    
    //
    //NEW CHANNEL BLOCK
    //
    if(makeChannel){
      //hardcoded for simplicity, determining which channel to route datamgs to is out of scope of assignment
      //send newchannelmsg, repsonse is name of a new server as string, use to construct client connection 
      MESSAGE_TYPE nC = NEWCHANNEL_MSG;
      chan.cwrite(&nC, sizeof(nC) );
      char nameBuf[30];
      chan.cread(nameBuf, sizeof(nameBuf));
      string newChanName(nameBuf);
      FIFORequestChannel newChan (newChanName, FIFORequestChannel::CLIENT_SIDE);
      
      //make/print several data requersts to new server
      datamsg *d2 = new datamsg(1, 0.016, 2);
      newChan.cwrite(d2, sizeof(datamsg) );
      data = 0;
      newChan.cread(&data, sizeof(double) );
      printf("data from new channel: = %lf \n", data);
      delete d2;
      datamsg *d3 = new datamsg(3, 0.040, 1);
      newChan.cwrite(d3, sizeof(datamsg) );
      data = 0;
      newChan.cread(&data, sizeof(double) );
      printf("data from new channel: = %lf \n", data);
      delete d3;
      datamsg *d4 = new datamsg(8, 0.056, 1);
      newChan.cwrite(d4, sizeof(datamsg) );
      data = 0;
      newChan.cread(&data, sizeof(double) );
      printf("data from new channel: = %lf \n", data);
      delete d4;
      
      //do not forget to close connection
      MESSAGE_TYPE n = QUIT_MSG;
      newChan.cwrite(&n, sizeof(MESSAGE_TYPE) );
    }
    
    
    
    
    
    //
    //QUITMSG BLOCK
    //
    //sends message to server to end connection
    //ensure other connections are severed for any subchannels in relevant section
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite(&m, sizeof(MESSAGE_TYPE) );
    
    sleep(1);
   
}
