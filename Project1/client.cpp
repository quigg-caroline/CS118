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
      cerr << "ERROR: timeout" << endl;
      close (sockfd);
      exit(1);
    }
  sleep(3);
  close(sockfd);
  exit(1);
    
  
}
