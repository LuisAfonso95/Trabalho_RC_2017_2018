//===================================================================
// program server2_tcp
// tcp server (receives from and sends msg to client)
// usage (command line):  server2_tcp  portnumber
// changed by Paulo Coimbra, 2014-10-25
/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
//===================================================================
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <semaphore.h>




#include "filelist_util.h"
//===================================================================
void dostuff(int); /* function prototype */
//===================================================================
// error messages: print message and terminate program
void error(char *msg) {
    perror(msg);
    exit(1);
}
//===================================================================






int socketGetFirstString(int sockfd, char *buffer, int max_size){
  //char buffer[256];
  bzero(buffer,max_size);
  char a = 0;
  int size=0;

  do{
    int n = read(sockfd,&a,1);
    if (n < 0){
      printf("ERROR reading from socket");
      exit(0);
    }

    if(n != 0){
      buffer[size] = a;
      size++;
      if(size == max_size)
        break;
    }
    else{
      size++;
      break;
    }


  }while(a != 0);
  buffer[size] = 0;

  return size;

}

//semaphores for acessing the files
//sem_t mutex;
sem_t stop_writers;

#define MAX_PORT_RANGE 65535 
#define MIN_PORT_RANGE 49152
/* Fixed name of the event list file */
#define EVENT_LIST_FILE "Event_List"
int main(int argc, char *argv[]) {


    if(argc < 2){
      printf("Insert port number as argument\n");
      return -1;
    }
    int portnumb = atoi(argv[1]);
    if(portnumb < MIN_PORT_RANGE || portnumb > MAX_PORT_RANGE){
      printf("Insert port number betwee %d and %d\n",MIN_PORT_RANGE, MAX_PORT_RANGE);
      return -1;
    }
    /* Create files for each event */
    char buffer[30];
    int i = 0;
    while(GetEventName(EVENT_LIST_FILE, buffer, 30, i) > 0 ){
      CreateFile(buffer);
      i++;
    }


    //create the semaphore to access the files
    /*if( sem_init(&(mutex), 0, 1) == -1) {
        printf("Error: %s\n", strerror(errno));
        exit(0);  
    }*/
    if( sem_init(&(stop_writers), 0, 1) == -1) {
        printf("Error: %s\n", strerror(errno));
        exit(0);
    }

    printf("Starting up the server\n");



    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;  //server addresses data

    //---creates tcp welcome socket (stream)...
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = portnumb;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
                  error("ERROR on binding");
  //---waits for clients
     listen(sockfd,5); 
     clilen = sizeof(cli_addr);



    //---forever cicle for clients
     printf("Server started\n");
    while (1) {
    //---accepts a new client
       newsockfd = accept(sockfd, 
             (struct sockaddr *) &cli_addr, &clilen);
       if (newsockfd < 0) error("ERROR on accept");
       pid = fork();
       if (pid < 0) error("ERROR on fork");
       if (pid == 0) {  // child (new) process to attend client
           close(sockfd); // sockfd belongs to father process
           dostuff(newsockfd);
           exit(0);
       } else // parent (old) process that keeps wainting for clients
           close(newsockfd); // newsockfd belongs to child process
    } //while
    return 0; /* we never get here */

}


void dostuff (int sock) {
   int n;
   char buffer[256];
      
   /* Reads user name for this session/connection */
   bzero(buffer,256);
   socketGetFirstString(sock, buffer, 256);
   printf("User: %s\n",buffer);
   char User[30];
   int i = 0;
   for(i = 0; i < strlen(buffer) && i < 30-1; i++){
      User[i] = buffer[i];
   }
   User[i] = 0;




   /* Loop reading incoming code messages */
   while(1){

    /* Read incoming messages */
    bzero(buffer,256);
    int t = socketGetFirstString(sock, buffer, 256);

    sem_wait(&(stop_writers));
    /* ======== ERROR ========  */
    if(t < 0){

    }
    else{

      /* ======== Command STOP - option 0========  */
      if(strcmp(buffer, "STOP") == 0){
            printf("User %s, requested connection close\n", User);
            n = write(sock,"STOP",strlen("STOP")+1);
            if (n < 0) fprintf(stderr,"\n\nERROR reading from socket");
            else{
                if(strcmp(buffer, "STOP") == 0){
                  close(sock);
                  exit(0);
                }
                else{
                    printf("Error closing connection. Please try again\n");
                }  
            }

      }

      /* ======== Command LISTEVENTS - option 3 ========  */
      else if(strcmp(buffer, "LISTEVENTS") == 0){
           printf("Received command to send list of events\n");

          /* n = write(sock,"ACK",strlen("ACK")+1);
           if (n < 0) error("ERROR writing to socket");*/

           int i=0;
           while(GetEventFixedInfo(EVENT_LIST_FILE, buffer, 256, i) > 0 ){
              int seats_available = EventAvailability(EVENT_LIST_FILE, i); 
              char sendbuffer[256];
              sprintf(sendbuffer, "%s, %d seats available", buffer, seats_available);
              n = write(sock,sendbuffer,strlen(sendbuffer)+1);
              if (n < 0) error("ERROR writing to socket");
              i++;
            }
            n = write(sock,"END",strlen("END")+1);
            if (n < 0) error("ERROR writing to socket");
      }

      /* ======== Command REGISTEREVENT - option 4 ========  */
      else if(strcmp(buffer, "REGISTEREVENT") == 0)
      {

            /* get event number */
            bzero(buffer,256);
            int t = socketGetFirstString(sock, buffer, 256);
            if (t < 0) error("ERROR reading from socket");
            int event = atoi(buffer);


            /* get number of seats */
            bzero(buffer,256);
            t = socketGetFirstString(sock, buffer, 256);
            if (t < 0) error("ERROR reading from socket");
            int seats = atoi(buffer);
            printf("User: %s, selected event %d, number of seats %d\n",User, event, seats);
           


            /* if number of seats is a valid number */
            if(seats > 0){
              /* Try to add request to registry */
              t = EventAddRegistry(EVENT_LIST_FILE, User, seats, event);
              if(t == ALREADY_REGISTERED){
                /* If the user already has a entry in the event, send error message */
                printf("ALREADYREGISTERED\n");
                n = write(sock,"ALREADYREGISTERED",strlen("ALREADYREGISTERED")+1);
                if (n < 0) printf("ERROR writing to socket\n");
              }
              /* == Case of not enough seats available */
              else if(t == NO_SEATS){
                printf("NO_SEATS\n");
                n = write(sock,"NO_SEATS",strlen("NO_SEATS")+1);
                if (n < 0) printf("ERROR writing to socket\n");
              }
              else{

                /* send confirmation of success */
                
                int number_of_regist = EventCountRegistry(EVENT_LIST_FILE, event);

                 bzero(buffer,256);
                int t = GetEventFixedInfo(EVENT_LIST_FILE, buffer, 256, event);

                if(t == EVENT_NOT_FOUND){
                  printf("EVENT_NOT_FOUND\n");
                  n = write(sock,"EVENT_NOT_FOUND",strlen("EVENT_NOT_FOUND")+1);
                  if (n < 0) printf("ERROR writing to socket\n");
                }
                else if(t < 0){
                  n = write(sock,"ERROR",strlen("ERROR")+1);
                  if (n < 0) printf("ERROR writing to socket\n");
                  printf("Error getting info\n");
                }
                else{
                  char writebuffer[256]; bzero(writebuffer,256);
                  sprintf(writebuffer, "Success, %d seats registered to: %s, with a total of %d seats already reserved\n",seats, buffer, number_of_regist);
                  n = write(sock,writebuffer,strlen(writebuffer)+1);
                  if (n < 0) error("ERROR writing to socket");
                }
              }
            }
            else{
                  n = write(sock,"ERROR",strlen("ERROR")+1);
                  if (n < 0) printf("ERROR writing to socket\n");
                  printf("Error, number of seats <= 0\n");
            }
            


      }

      /* ======== Command SHOWREGLIST - option 5 ========  */
      else if(strcmp(buffer, "SHOWREGISTERED") == 0)
      {
        printf("User %s, requested to see registered events\n ", User);
        int t = 0;
        int k = 0;

        char userinfo[256];
        
        do{
          bzero(userinfo,256);
          t = SearchUserInFile(EVENT_LIST_FILE, User, k, userinfo);
          if(t >= 0){
                char name[64];
                bzero(name,64);
                GetEventName(EVENT_LIST_FILE, name, 64, k);
                char sendbuff[256];
                bzero(sendbuff,256);
                sprintf(sendbuff, "Event: %s; %s", name, userinfo);
                printf("Found: %s\n",sendbuff);
                n = write(sock,sendbuff,strlen(sendbuff)+1);
                if (n < 0) error("ERROR writing to socket");
          }
          k++;
        }while(t >= 0 || t == USER_NOT_FOUND);
        n = write(sock,"END",strlen("END")+1);
        if (n < 0) error("ERROR writing to socket");
      }


      /* ======= Command Unknow command  ======== */
      else{
        printf("Unknow command\n");
        n = write(sock,"ERROR",strlen("ERROR")+1);
        if (n < 0) printf("ERROR writing to socket\n");
      }

    }

    sem_post(&(stop_writers));
   }

   //---sends message to client...








   
}
//===================================================================
//===================================================================

