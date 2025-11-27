
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define NCONNECTIONS 5
#define PORT 3000
// #define PORT 3001

int main() {
  int socketfd = {0}, c_socketfd;
  struct sockaddr_in sockaddr, c_sockaddr;
  socklen_t c_addr_len = sizeof(c_sockaddr);
  char buffer[256];
  int n;
  // socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  socketfd = socket(PF_INET, SOCK_STREAM, 0);
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(PORT);
  sockaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

  if (bind(socketfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1)
    perror("bind error");
  
  printf("Listening on port %d\n", PORT);
  // fflush(stdout);
  
  listen(socketfd, NCONNECTIONS);
  

  while (1) {
    c_socketfd = accept(socketfd, (struct sockaddr*)&c_sockaddr, &c_addr_len);
    if (fork() == 0) {

    close(socketfd);

    if (c_socketfd == -1)
        perror("accept");

      bzero(buffer, sizeof(buffer));

      while (1) {
        n = read(c_socketfd, buffer, sizeof(buffer));
        if (n == -1) {
          perror("reading from client");
          exit(1);
        }
        if (strncmp(buffer, "shutdown", 8) == 0) {
          printf("[SERVER] Exiting... \n");
          close(c_socketfd);
          kill(getppid(), SIGINT);
          exit(0);
        } else {
          // printf("Buffer: %s\n", buffer);
        }
          // break;

        printf("[SERVER] Received message: %s\n", buffer);
        // fprintf(stdout, "Received message: %s\n", buffer);
        // fflush(stdout);
        bzero(buffer, sizeof(buffer));

        strcpy(buffer, "Echo zo servera");
        if (write(c_socketfd, buffer, sizeof(buffer) == -1))
          perror("writing to client");

        bzero(buffer, sizeof(buffer));


        // exit(0);
      }
    }
  }

  close(socketfd);
  close(c_socketfd);
  return 0;
}
