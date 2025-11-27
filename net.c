#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#define MYPORT 3490

int main(void) {
  int s;
  struct sockaddr_in my_addr = {0};
  int sin_size;
  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s == -1) perror("socket");
  else {
    printf("%d\n", s);
  }
  my_addr.sin_family = AF_INET; //naplnenie struktury sockaddr_in
  my_addr.sin_port = htons(MYPORT);
  // my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_addr.s_addr = htonl(inet_addr("127.0.0.1"));
  // bzero(&(my_addr.sin_zero), 8);

  if(bind(s,(struct sockaddr *)&my_addr, sizeof(struct sockaddr))==-1){

  // listen(s, SOMAXCONN);
  listen(s, 5);

  // accept(int fd, struct sockaddr *restrict addr, socklen_t *restrict addr_len)

  perror("bind");
  exit(1);
}

  return 0;
}
