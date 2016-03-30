/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //defines various data types
#include <sys/socket.h> //includes a number of definitions of structures needed for sockets.
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses.
#include <string.h>
#include<time.h>
#include <stdbool.h>
void error(char *msg)
{
    perror(msg); //produces a short  error  message  on  the  standarderror describing
    exit(1);
}
void game(int sockfd, int id){
  char buffer[256];
  int n;
  bool loop = false;
  srand ( time(NULL) );
  int random = (rand() % 100) + 1;
  printf("Prisijunge klientas %d\n",id);
  printf("Klientui %d sugalvotas skaicius: %d \n", id, random);
  do{
    /*answer if loop do again */
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    if (strcmp(buffer,"1") == 0){
      loop = true;
      bzero(buffer,256);
      n = read(sockfd,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      printf("Kliento %d spetas skaicius: %s\n", id, buffer);
      int num = atoi(buffer);
      bzero(buffer,256);
      if(random == num){
        buffer[0] = 'e'; //equal
      }
      if(random > num){
        buffer[0] = 'g'; //more
      }
      if (random < num) {
        buffer[0] = 'l'; //less
      }
      n = write(sockfd,buffer,1);
      if (n < 0) error("ERROR writing to socket");  
    }else{
      loop = false;
    }
  }while(loop == true);
}
int main(int argc, char *argv[])
{
     int sockfd, newsockfd;
     int portno, pid;
     int clilen; //size of the address of the client
     struct sockaddr_in serv_addr; //structure containing an internet address.
     struct sockaddr_in cli_addr; //structure contain the address of the server
     int n;
     if(argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0); // first param. address domain of the socket, second type of socket, third protocol.(TCP,UDP).
     if (sockfd < 0) // if fails socket(); return -1
        error("ERROR opening socket");
     /* initialize socket structure */
     bzero((char *) &serv_addr, sizeof(serv_addr)); //first param. pointer to buffer, second size of buffer.) Sets all values in a buffer to zero
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET; //contains a code for the address family. It should always be set to the symbolic constant AF_INET.
     serv_addr.sin_addr.s_addr = INADDR_ANY; // ip address of the host
     serv_addr.sin_port = htons(portno); // convert this to network byte order using the function htons()
     /*bind the host address using bind() call.*/
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     /* start listening for the clients */
     listen(sockfd,5); //system call allows the process to listen on the socket for connections.
     clilen = sizeof(cli_addr);
     printf("Serveris laukia prisijungimo...\n");
     int i = 0; // Client ID
     while(1){
       newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
       i++; // Client ID
       if (newsockfd < 0) 
          error("ERROR on accept");
       pid = fork();
       if (pid < 0){
         error("ERROR on fork");
       }     
       if (pid == 0)  {
         close(sockfd);
         game(newsockfd,i);
         exit(0);
       }else close(newsockfd);
     }
     close(sockfd);
     return 0;
}