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

#define BACKLOG 10
using namespace std;

int main(int argc, char** argv)
{

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
  int sockfd;
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
  int count  = 0;
  struct sockaddr_storage clientAddrs[BACKLOG];
  int client_fds[BACKLOG];
  socklen_t addr_size = sizeof(clientAddrs[count]);
  client_fds[count] = accept(sockfd, (struct sockaddr*) &clientAddrs[count],&addr_size);
  if (client_fds[count] == -1)
    {
      cerr<< "ERROR: accept error " << strerror(errno) << endl;
      exit(1);
    }
  count++;

  cout<< "Accepted connection number " << count << endl;

  //open file to write to
  string title = argv[2];
  title = title + "/" + to_string(1) + ".file";

  ofstream file;
  file.open(title);
  
  //read from client
  char buffer[100];
  memset(&buffer, 0 , sizeof(buffer));
  read(client_fds[0], buffer, sizeof(char));

  file << buffer;
  
  
  for (int i =0 ; i< count; i++)
    {
      close(client_fds[i]);
    }

  file.close();
  
  //free serv addr
  close (sockfd);
  freeaddrinfo(servinfo);
  return 0;
}
