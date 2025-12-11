#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/signal.h>

#define NCONN 5

// TODO: shared memory
int clisockfd[NCONN];
int pipefd[2];

void error(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

void broadcast_msg(int sig) {
  char buffer[100];
  int n;
  if ((n = read(pipefd[0], buffer, sizeof(buffer))) == -1)
    error("reading from pipe");

  buffer[n] = '\0'; // null terminate raw bytes
  for (int i = 0; i < NCONN; i++) {
    if (clisockfd[i] != 0) {
      printf("[BROADCAST] Attempting to write to client %d (fd=%d)\n", i, clisockfd[i]);
      int res = write(clisockfd[i], buffer, sizeof(buffer));
      if (res == -1) error("writing to socket failed");
      printf("[SERVER] sent broadcast message...\n");
    }
  }  
}

int main(int argc, char *argv[]) {
  int sockfd, tempsockfd, portno, clilen, n;
  char buffer[256];
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  if (pipe(pipefd) == -1) {
    error("creating pipe failed");
  }
  signal(SIGUSR1, broadcast_msg);
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  bzero(&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  listen(sockfd, NCONN);
  clilen = sizeof(cli_addr);

  while (1) {
    
    tempsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
    printf("client connected\n");
    if (tempsockfd < 0) error("ERROR on accept");
    int i;
    // TODO: handle cleanup properly
    for (i = 0; i < 5; i++) {
      if (clisockfd[i] == 0) {
        clisockfd[i] = tempsockfd;
        break;
      }
    }
    
    if (fork() == 0) {
      close(sockfd);
      close(pipefd[0]);
      while (1) {
        bzero(buffer,256);
        n = read(clisockfd[i],buffer,255);
        if (n <= 0) {
          if (n == 0) {
              printf("Client disconnected (EOF)\n");
              exit(EXIT_SUCCESS);
          }
          else {
              printf("Read error\n");
          }
        }
        printf("[SERVER] received message: %s\n",buffer);
        write(pipefd[1], buffer, strlen(buffer));
        kill(getppid(), SIGUSR1);
      }
      close(pipefd[1]);
      close(tempsockfd);
      clisockfd[i] = 0;
      exit(0);
    }
  }
  return EXIT_SUCCESS;
}
