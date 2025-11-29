#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
void error(char *msg)
{ //tato funkcia sa vyuziva ked systemove volanie zlyha
perror(msg); //vypise spravu o chybe a ukonci program server
exit(1);
}
int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno, clilen, n; //pomocne premenne
  char buffer[256]; //buffer pre ulozenie znakov zo socketu
  struct sockaddr_in serv_addr; //obsahuje adresu servera
  struct sockaddr_in cli_addr; //obsahuje adresu klienta
  if (argc < 2) { //kontrola poctu argumentov
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  //vytvorenie socketu
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  //naplnenie struktury sockaddr_in
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  //zviazanie socketu sluzbou bind()
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  listen(sockfd,5); //vytvorime si front poziadaviek
  clilen = sizeof(cli_addr); //velkost struktury adresy klienta
  //akceptovanie spojenia
  newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
  if (fork() == 0) {
    close(sockfd);
    if (newsockfd < 0) error("ERROR on accept");
    while (1) {
      bzero(buffer,256); //spracovanie dat od klientov
      n = read(newsockfd,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      printf("[SERVER] received message: %s\n",buffer);
      n = write(newsockfd,"I got your message",18);
      if (n < 0) error("ERROR writing to socket");
    }
    exit(0);
  }
  return 0;
}
