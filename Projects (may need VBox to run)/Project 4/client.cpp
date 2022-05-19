#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
#include "TCPreqchannel.h"
#include <time.h>
#include <thread>
using namespace std;

/* Obsolete, from Project 3
FIFORequestChannel* create_new_channel(FIFORequestChannel* mainChan){
  char chanName[1024];
  MESSAGE_TYPE m = NEWCHANNEL_MSG;
  mainChan->cwrite(&m, sizeof(m));
  mainChan->cread(chanName, 1024);
  FIFORequestChannel* newChan = new FIFORequestChannel (chanName, FIFORequestChannel::CLIENT_SIDE);
  return newChan;
} 
*/

void patient_thread_function(int n, int pNum, BoundedBuffer* reqBuf){
  datamsg d(pNum, 0.0, 1);
  double resp = 0;
  
  for(int i=0; i<n; i++){
    reqBuf->push( (char*)&d, sizeof(datamsg) );
    d.seconds += 0.004;
  }
}

void worker_thread_function(TCPRequestChannel* chan, BoundedBuffer* reqBuf, HistogramCollection* hc, int mBuf){
  char buf[1024];
  double resp = 0;
  char recvbuf[mBuf];
  
  while(true){
    reqBuf->pop(buf, 1024);
    MESSAGE_TYPE* m = (MESSAGE_TYPE *) buf;
    
    if(*m == DATA_MSG){
      chan->cwrite(buf, sizeof(datamsg) );
      chan->cread(&resp, sizeof(double));
      hc->update( ( (datamsg *)buf )->person, resp);
    }else if(*m == QUIT_MSG){
      chan->cwrite(m, sizeof(MESSAGE_TYPE) );
      delete chan;
      break;
    }else if(*m == FILE_MSG){
      filemsg* fm = (filemsg*) buf;
      string fname = (char* )(fm +1);
      int sz = sizeof(filemsg) + fname.size() +1;
      
      chan->cwrite(buf, sz);
      chan->cread(recvbuf, mBuf);
      
      string reFilename = "recv/" + fname;
      FILE* fp = fopen(reFilename.c_str(), "r+");
      
      fseek(fp, fm->offset, SEEK_SET);
      fwrite(recvbuf, 1, fm->length, fp);
      fclose(fp);
    }
  }
}

void file_thread_function(string filename, BoundedBuffer* reqBuf, TCPRequestChannel* chan, int mBuf){
  //create
  string reFilename = "recv/" + filename;
  
  char buf[1024];
  filemsg f(0,0);
  memcpy(buf, &f, sizeof(f));
  strcpy(buf + sizeof(f), filename.c_str());
  chan->cwrite(buf, sizeof(f) + filename.size() +1);
  __int64_t filelength;
  chan->cread(&filelength, sizeof(filelength));
  FILE* fp = fopen(reFilename.c_str(), "wb");
  fseek(fp, filelength, SEEK_SET);
  fclose(fp);
  
  //gen filemsgs
  filemsg* fmsg = (filemsg*) buf;
  __int64_t remlen = filelength;
  
  while(remlen > 0){
    fmsg->length = min(remlen, (__int64_t)mBuf);
    reqBuf->push(buf, sizeof(filemsg) + filename.size() +1);
    fmsg->offset += fmsg->length;
    remlen -= fmsg->length;
  }
}



int main(int argc, char *argv[]){
  int n = 100;          //default number of requests per "patient"
  int p = 10;           // number of patients [1,15]
  int w = 100;          //default number of worker threads
  int b = 500;           // default capacity of the request buffer, you should change this default
  int m = MAX_MESSAGE;  // default capacity of the message buffer
  srand(time_t(NULL));
  
  int opt;
  string filename, host, port;
  bool getFile = false, passM = false;
  
  //Process arguments to change above parameters
      while ((opt = getopt(argc, argv, "n:p:w:b:m:f:h:r:")) != -1) {
      switch (opt) {
        case 'n':
          n = atoi(optarg);
          break;
        case 'p':
          p = atoi(optarg);
          break;
        case 'w':
          w = atoi(optarg);
          break;
        case 'b':
          b = atoi(optarg);
          break;
        case 'm':
          passM = true;
          m = atoi(optarg);
          break;
        case 'f':
          filename = optarg;
          getFile = true;
          break;
        case 'h':
          host =  optarg;
          break;
        case 'r':
          port =  optarg;
          break;
      }
    }
  
  /* Obsolete, from Project 3
  //start server as parent, pass in m 
  int pid = fork();
  if (pid == 0){
  // modify this to pass along m
      
    if(passM){
      execl ("server", "server", "-m", (char*)m ,(char *)NULL);
    }else{
      execl ("server", "server", (char *)NULL);
    }
  }
  */
  
  TCPRequestChannel* chan = new TCPRequestChannel(host, port, FIFORequestChannel::CLIENT_SIDE);
  BoundedBuffer request_buffer(b);
  HistogramCollection hc;
  
  ///create histograms for each person
  for(int i=0; i<p; i++){
    Histogram* h = new Histogram(10, -2.0, 2.0);
    hc.add(h);
  }
  
  //create worker channels 
  TCPRequestChannel* workChannels [w];
  for(int i=0; i<w; i++){
    workChannels[i] = new TCPRequestChannel(host, port, FIFORequestChannel::CLIENT_SIDE);
  }
  
  
  struct timeval start, end;
  gettimeofday (&start, 0);
  
  
  //Start all threads here 
  
  //uses file_thread_function if -f [filename] used as arg, just patient data otherwise
  if(getFile){
    
    thread filethreads(file_thread_function, filename, &request_buffer, chan, m);
    
    thread workers[w];
    for(int i=0; i<w; i++){
      workers[i] = thread( worker_thread_function, workChannels[i], &request_buffer, &hc, m);
    }
    
    filethreads.join();
    
    for(int i=0; i<w; i++){
      MESSAGE_TYPE q = QUIT_MSG;
      request_buffer.push( (char*)&q, sizeof(q) );
    }
    
    for(int i=0; i<w; i++){
      workers[i].join();
    }
    
  }else{
    
    thread patient[p];
    for(int i=0; i<p; i++){
      patient[i] = thread(patient_thread_function, n, i+1, &request_buffer);
    }
    
    thread workers[w];
    for(int i=0; i<w; i++){
      workers[i] = thread( worker_thread_function, workChannels[i], &request_buffer, &hc, m);
    }
    
    for(int i=0; i<p; i++){
      patient[i].join();
    }
    
    for(int i=0; i<w; i++){
      MESSAGE_TYPE q = QUIT_MSG;
      request_buffer.push( (char*)&q, sizeof(q) );
    }
    
    for(int i=0; i<w; i++){
      workers[i].join();
    }
  }
  
  
  gettimeofday (&end, 0); 
  
  // print the results
  hc.print ();
  
  //print timecheck block
  int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
  int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
  cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;
  
  MESSAGE_TYPE q = QUIT_MSG;
  chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
  cout << "All Done!!!" << endl;
  delete chan;
  
}
