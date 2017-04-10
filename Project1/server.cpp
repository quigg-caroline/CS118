#include <string>
#include <thread>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char* argv)
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
      std::cerr << "ERROR: getaddrinfo error " << gai_strerror(status) << std::endl;
      exit(1);
    }

  //create a socket that uses TCP IP
  int sockfd;
  if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, 0)) < 0 )
    {
      std::cerr << "ERROR: socket error " << strerror(errno) << std::endl;
      exit(1);
    }

  //allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      std::cerr << "ERROR: setsockopt error " << strerror(errno) << std::endl;
      exit(1);
    }

  //bind socket to address
  if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
      std::cerr << "ERROR: bind error " << strerror(errno) << std::endl;
      exit(1);
    }

  //set socket to listen
  if (listen(sockfd, 10) == -1)
    {
      std::cerr << "ERROR: listen error " << strerror(errno) << std::endl;
      exit(1);
    }

  
  
  //free serv addr
  close (sockfd);
  freeaddrinfo(servinfo);
}
