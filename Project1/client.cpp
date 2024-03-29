#include <string>
#include <thread>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>
#include <fcntl.h>

using namespace std;
int main(int argc, char* argv[])
{
  if (argc != 4)
    {
      cout << "ERROR: Wrong arguments " <<endl;
      exit(1);
    }
  
  struct addrinfo hints;
  struct addrinfo* res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP

  //get server information
  if(getaddrinfo(argv[1], argv[2], &hints, &res) == -1)
    {
      cerr << "ERROR: gettaddrinfo error - " << strerror(errno) << endl;
      exit(1);
    }

  //make a socket
  int sockfd = socket(res->ai_family, res->ai_socktype, 0);
  if (sockfd < 0)
    {
      cerr << "ERROR: socket error - " << strerror(errno) << endl;
      exit(1);
    }

  //connect to server

  //set timeout val
  struct timeval timeout;
  timeout.tv_sec =10;
  timeout.tv_usec = 0;

  //set fd set
  fd_set set;
  FD_ZERO(&set);
  FD_SET(sockfd, &set);

  fcntl(sockfd, F_SETFL, O_NONBLOCK);

  //connect
  if ( connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
      if (errno != EINPROGRESS)
	{
	  cerr << "ERROR: connect error - " << strerror(errno) << endl;
	  exit(1);
	}
    }

  //check for timeout
  int status =  select (sockfd+1, NULL, &set, NULL, &timeout);
  if (status == 0 )
    {
      cerr << "ERROR: connection timeout" << endl;
      close (sockfd);
      exit(1);
    }


  //open file and get its size
  FILE* fs = fopen(argv[3], "r");
  fseek(fs, 0, SEEK_END);
  unsigned long size = ftell(fs);
  fseek(fs, 0, SEEK_SET);
  //copy to buffer
  char* buffer = (char*)malloc(sizeof(char) * size);
  long long r = fread(buffer, sizeof(char), size, fs);
  
  
  r = 0;

  //write to server, multiple times if needed
  while (r < (long long)size && r != -1)
    {
      //check if server can be written to
      FD_ZERO(&set);
      FD_SET(sockfd, &set);
      select(sockfd+1, NULL, &set, NULL, &timeout);
      if (FD_ISSET(sockfd, &set))
	{
	  //write to server
	  size = size - r;
	  r = write (sockfd, buffer, size);
	}
      else
       {
	 //connection timeout
	 close(sockfd);
	 cout << "ERROR: send timeout " <<endl;
	 exit(1);
       }
    }

  if (r == -1)
    {
      cout << "ERROR: send error - " << strerror(errno) << endl;
      exit(1);
    }
  
  close(sockfd);
  exit(1);
    
  
}
