#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define clear() printf("\033[H\033[J");
void error(char *msg);
int check_int();
bool meniu(int sockfd);
bool guess_num(int sockfd);
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    int choice;
    
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    /*create socket */
    bool connect_again = true;
    do{
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) 
          error("ERROR opening socket");
      server = gethostbyname(argv[1]);
      if (server == NULL) {
          fprintf(stderr,"ERROR, no such host\n");
          exit(0);
      }
      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr, 
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);
      serv_addr.sin_port = htons(portno);
      /* connect to server */
      if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
         error("ERROR connecting");
      }
      else{
        printf("Prie serverio prisijungta sekmingai.\n");
        connect_again = meniu(sockfd);  
      } 
      
      if(connect_again  == true){
        printf("Ar norite zaisti dar karta? \n");
        printf("1: Taip \n");
        printf("bet koks simbolis: Ne \n");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        if(strcmp(buffer,"1\n") == 0){
          connect_again  = true;
        }else{
         connect_again  = false;
        }
      }
    }while(connect_again  == true); 
    return 0;
}
void error(char *msg)
{
    perror(msg);
    exit(0);
}
int check_int(){
    int num = 0;
    while(1){
      if((scanf("%d",&num) == 1)&& (num >0) && (num <= 100) && (getchar() == '\n') ){ 
        break;
      }
      else{
        printf("Klaidingai ivedete skaiciu.\n");
        printf("Iveskite ji is naujo.\n");
        while(getchar()!= '\n');
      }
    }
    return (num);
}
bool meniu(sockfd){
  char buffer[256];
  bool loop = true;
  int n;
  bool guess;
  do{
    printf("Pasirinkite:\n");
    printf("Speti: 1\n");
    printf("Iseiti: bet koks simbolis\n");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer)-1);
    if (n < 0){
      error("ERROR writing to socket");
    }
    clear();
    if(strcmp(buffer,"1\n") == 0){
      loop = guess_num(sockfd);
      guess = true;
    }else{
      loop = false;
      guess = false;
    }
  }while(loop == true);
  return (guess);
}
bool guess_num(sockfd){
    char buffer[256];
    int num;
    int n;
    printf ("Spekite skaiciu  nuo 1 iki 100. \n");
    num = check_int();
    bzero(buffer,256);
    sprintf(buffer, "%d", num);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0){
      error("ERROR writing to socket");
    }
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0){
       error("ERROR reading from socket");
    }
    if(buffer[0] == 'e'){
      printf("Jus atspejote skaiciu.\n");
      return false; 
    }
    if(buffer[0] == 'l'){
      printf("Spejamas skaicius mazesnis.\n");
      return true;
    }
    if(buffer[0] == 'g'){
      printf("Spejamas skaicius yra didesnis.\n");
      return true;
    }
}