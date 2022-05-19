#ifndef TCPreqchannel_h
#define TCPreqchannel_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
using namespace std;


class TCPRequestChannel{
private:
  int sockfd;
  
  int server (string port){
	  //int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *serv;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &serv)) != 0) {
        cerr  << "getaddrinfo: " << gai_strerror(rv) << endl;
        return -1;
    }
	  if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
        perror("server: socket");
		    return -1;
    }
    if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
		close(sockfd);
		perror("server: bind");
		return -1;
	  }
    freeaddrinfo(serv); // all done with this structure

    if (listen(sockfd, 20) == -1) {
        perror("listen");
        exit(1);
    }
  }
  
  int client (string server_name, string port){
	  struct addrinfo hints, *res;
	  //int sockfd;

	  // first, load up address structs with getaddrinfo():
	  memset(&hints, 0, sizeof hints);
	  hints.ai_family = AF_UNSPEC;
	  hints.ai_socktype = SOCK_STREAM;
	  int status;
	  //getaddrinfo("www.example.com", "3490", &hints, &res);
	  if ((status = getaddrinfo (server_name.c_str(), port.c_str(), &hints, &res)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(status) << endl;
        return -1;
    }

	  // make a socket:
	  this->sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	  if (sockfd < 0){
	  	perror ("Cannot create scoket");	
	  	return -1;
	  }

	  // connect!
	  if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0){
	  	perror ("Cannot Connect");
	  	return -1;
	  }
	  return 0;
  }
  
public:
  TCPRequestChannel(const string _host, const string _port, int _side){
    if(_side == 0){
      server(_port);
    }else{
      client(_host, _port);
    }
  }
  
  TCPRequestChannel(int _s){
    sockfd = _s;
  }
  
  ~TCPRequestChannel(){ close(sockfd); }
  
  int cread(void* buf, int len){
    return recv(sockfd, buf, len, 0);
  }
  
  int cwrite(void* buf, int len){
    return send(sockfd, buf, len, 0);
  }
  
  int getsocket(){
    return sockfd;
  }
  
};

#endif
