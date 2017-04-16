#include <string>
#include <thread>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <fstream>
#include <csignal>

#define BACKLOG 10
using namespace std;

int sockfd;
int count  = 0;
struct sockaddr_storage clientAddrs[BACKLOG];
int client_fds[BACKLOG];
socklen_t addr_size = sizeof(clientAddrs[count]);

//handle SIGTERM and SIGQUIT
void sig_handler(int signum)
{
  //close open sockets
  close(sockfd);
  for(int i = 0 ; i<count ; i++)
    {
      close(client_fds[i]);
    }
  exit(0);
}

//read data from a specific client


int main(int argc, char** argv)
{

  //set up handler for SIGQUIT/SIGTERM
  signal(SIGQUIT, sig_handler);
  signal(SIGTERM, sig_handler);
  
  //get addr info
  int status;
  struct addrinfo hints;
  struct addrinfo* servinfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; //IPv4
  hints.ai_socktype = SOCK_STREAM; //TCP stream sockets
  hints.ai_flags = AI_PASSIVE; //fill in IP for me
  
  if ((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0)
    {
      cerr << "ERROR: getaddrinfo error " << gai_strerror(status) << endl;
      exit(1);
    }

  //create a socket that uses TCP IP
  if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, 0)) < 0 )
    {
      cerr << "ERROR: socket error " << strerror(errno) << endl;
      exit(1);
    }

  //allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      cerr << "ERROR: setsockopt error " << strerror(errno) << endl;
      exit(1);
    }

  //bind socket to address
  if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
      cerr << "ERROR: bind error " << strerror(errno) << endl;
      exit(1);
    }

  //set socket to listen
  if (listen(sockfd, BACKLOG) == -1)
    {
      cerr << "ERROR: listen error " << strerror(errno) << endl;
      exit(1);
    }

  //accept new connection(s)
  client_fds[count] = accept(sockfd, (struct sockaddr*) &clientAddrs[count],&addr_size);
  if (client_fds[count] == -1)
    {
      cerr<< "ERROR: accept error " << strerror(errno) << endl;
      exit(1);
    }
  count++;

  //start new thread for client

  cout<< "Accepted connection number " << count << endl;

  //open file to write to
  string title = argv[2];
  title = title + "/" + to_string(1) + ".file";

  ofstream file;
  file.open(title);

  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  
  //see if there is data from client
  fd_set set;

  while (1)
    {
      FD_ZERO(&set);
      FD_SET(client_fds[0], &set);
  
      select(client_fds[0] + 1, &set, NULL, NULL, &timeout);

      if (FD_ISSET(client_fds[0], &set))
	{
	  //read from client
	  char buffer[100];
	  memset(&buffer, 0 , sizeof(buffer));
	  
	  long r  =recv(client_fds[0], buffer, sizeof(buffer),0);
	  //check if client terminated
	  if (r == 0)
	    {
	      break;
	    }
	  file << buffer;
	}

      else
	{
	  file << "ERROR: client aborted";
	  break;
	}
    }
    close(client_fds[0]);
    file.close();
  
  //free serv addr
  close(sockfd);
  freeaddrinfo(servinfo);
  return 0;
}
