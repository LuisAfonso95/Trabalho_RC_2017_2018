    //===================================================================
// program client_tcp (equal to intro.cclient_intro.c)
// tcp client (sends to and receive msg from server)
// usage (command line):  client_tcp  hostname  portnumber
// changed by Paulo Coimbra, 11.10.25, 2013-10-21
//===================================================================
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
//===================================================================
// error messages: print message and terminate program
void error(char *msg) {
    perror(msg);
    exit(0);
}


#define NUMBER_OF_OPTIONS 6
#define MAX_PORT_RANGE 65535 
#define MIN_PORT_RANGE 49152
/* Menu options */
const char options[NUMBER_OF_OPTIONS][40] = { 
                    "0. Exit\n",
                    "1.Set event server\n",
                    "2.Set port (49152-65535)\n",
                    "3.Get list of events\n",
                    "4.Make registration\n",
                    "5.Show registration list\n"
                };

//===================================================================

int WaitACK(int sockfd){
    char buffer[256];
    bzero(buffer,256);
    int n = read(sockfd,buffer,255);

    if (n < 0) fprintf(stderr,"\n\nERROR reading from socket");
    else if(strcmp("ACK", buffer) == 0) return 1;
    return 0;

}

int socketGetFirstString(int sockfd, char *buffer, int max_size){
  //char buffer[256];
  bzero(buffer,max_size);
  char a = 0;
  int size=0;

  do{
    int n = read(sockfd,&a,1);
    if (n < 0){
      printf("ERROR reading from socket");
      return -1;
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

int connected_to_server = 0; //flag to indicate if connection was established
int main(int argc, char *argv[]) {


    //Check if there is a argument. Should be the username
    if(argc < 2)
        return 0;


    int sockfd, portno, n;  //socket file descriptor, port number
    struct sockaddr_in serv_addr;  //server address data
    struct hostent *server;
    char buffer[256];  // data (bytes) to be sent to server

    char defaul_IP_Server[] =  "127.0.0.1";
    server = gethostbyname(defaul_IP_Server);
    unsigned int port_Server = 50000;


    /* Menu loop */
    while(1){

        /* Print header and options */
        printf("\nSetEvent Client 2017\n");
        printf("by Luis Afonso and Pedro Eugénio (DEEC-UC)\n");

        printf("\nCurrent event server: %s\n", server->h_name);
        printf("Current port: %d\n", port_Server);

        int i;  
        for(i = 0; i < NUMBER_OF_OPTIONS; i++){
            printf("%s",options[i]);
        }




        /* Wait for number input */            
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        int option = atoi(buffer);
        //char c = 'A';
        //while ((c = getchar()) != EOF && c != '\n');



        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        printf("Selected option:  %s\n", options[option]);

        /* Operation based on option selected */
        if(option < 0 || option > NUMBER_OF_OPTIONS-1){
            printf("Invalid option\n");
        }
        else if(option == 0){
            /* Exit option. Needs to close TCP connection if already established*/
            n = write(sockfd,"STOP",strlen("STOP")+1);
            if (n < 0) fprintf(stderr,"\n\nERROR writing to socket"); 
            n = socketGetFirstString(sockfd, buffer, 255);
            if (n < 0) fprintf(stderr,"\n\nERROR reading from socket");
            else{
                if(strcmp(buffer, "STOP") == 0){
                     close(sockfd);
                     exit(0);
                }
                else{
                    printf("Error closing connection. Please try again\n");
                }  
            }
        }
        else if(option == 1){
            /* Option 1 just asks for a new IP for the server */
             struct hostent *temp_server;
            while(1){
                printf("Please indicate new server IP or name\n");
                bzero(buffer,256);
                fgets(buffer,255,stdin);
              
                //remove new line char
                buffer[strlen(buffer)-1] = 0;

                temp_server = gethostbyname(buffer);  //uses DNS to know IP address
                if (temp_server == NULL) {
                    fprintf(stderr,"\n\nERROR, no such host\n");
                }
                else{
                    break;
                }

            }

            printf("new server: %s\n", temp_server->h_name);
            server = temp_server;
    


        }

        else if(option == 2){
            /* Option 2 just asks for a new port number of the server */
            int new_port = -1;
            while(1){
                printf("Please indicate new port number\n");
                bzero(buffer,256);
                fgets(buffer,255,stdin);
                 new_port = atoi(buffer);
                if(new_port <=MAX_PORT_RANGE && new_port>=MIN_PORT_RANGE ){
                    break;
                }
                else{
                    printf("\n\n\n\nInvalid port number. Please introduce a value between %d and %d\n", MIN_PORT_RANGE, MAX_PORT_RANGE);
                }

            }

            printf("new port: %d\n", new_port);
            port_Server = new_port;


        }
        else if(option == 3){
            /* Option 3 will establisha connection, if it still hasn't, and request event list */

            /* Connect to server if still hasn't and send user name*/
            if(connected_to_server == 0){

                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                bzero((char *) &serv_addr, sizeof(serv_addr)); //clears serv_addr
                serv_addr.sin_family = AF_INET; 
                bcopy((char *)server->h_addr, 
                     (char *)&serv_addr.sin_addr.s_addr,
                     server->h_length);
                serv_addr.sin_port = htons(port_Server); //portno must be in network format
                if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0) {
                    fprintf(stderr,"\n\nERROR connecting\n");
                
                }
                else{
                    n = write(sockfd,argv[1],strlen(argv[1])+1);
                    if (n < 0) fprintf(stderr,"\n\nERROR writing to socket");
                    else{
                        //if(WaitACK(sockfd) == 1) connected_to_server = 1;
                       
                      connected_to_server = 1  ;
                      printf("Connected to server\n");
                    } 


                }
            }
            /* If connected to server request list and receive it*/
            if(connected_to_server == 1){
                printf("Requesting event list\n");

                //need to send request for list "LISTEVENTS"
                 n = write(sockfd,"LISTEVENTS",strlen("LISTEVENTS")+1);
                 if (n < 0) fprintf(stderr,"\n\nERROR writing to socket");
                 //WaitACK(sockfd);
                                
                int counter = 0;
                while(1){
                    bzero(buffer,256);
                    n = read(sockfd,buffer,255);
                    if (n < 0) fprintf(stderr,"\n\nERROR reading from socket");
                    if(strcmp(buffer,"END") != 0){
                        printf("%d. %s\n",counter, buffer); 
                        counter++;
                    }   
                    else
                        break; 
                }
            }
                

            

        }
        else if(option == 4 && connected_to_server == 1){
            /* Option 4 will send command to register then send event number and number of seats*/

            //Missing send register command "REGISTEREVENT"
            n = write(sockfd,"REGISTEREVENT",strlen("REGISTEREVENT")+1);
            if (n < 0) fprintf(stderr,"\n\nERROR writing to socket");

            printf("Introduce the number of the event you want to register to: ");
            bzero(buffer,256);
            fgets(buffer,255,stdin);
            //int event = atoi(buffer);
            n = write(sockfd,buffer,strlen(buffer)+1);
            if (n < 0){ fprintf(stderr,"\n\nERROR writing to socket"); break;}

            printf("\n Now introduce the number of people to register: ");
            bzero(buffer,256);
            fgets(buffer,255,stdin);
            //int seats = atoi(buffer);   
            n = write(sockfd,buffer,strlen(buffer)+1);
            if (n < 0){ fprintf(stderr,"\n\nERROR writing to socket"); break;}

            //Read return string from server
            bzero(buffer,256);
            int t = socketGetFirstString(sockfd, buffer, 256);
            if (t < 0) printf("ERROR reading from socket\n");
            printf("\n%s\n\n",buffer);

        }

        else if(option == 5 && connected_to_server == 1){
            /* Option 5 option prints on screen the registrations performed by the user.*/

            if(connected_to_server == 1){
                printf("Requesting event registrations\n");

                //need to send request for list "REGISTEREVENT"
                 n = write(sockfd,"REGISTEREVENT",strlen("REGISTEREVENT")+1);
                 if (n < 0) fprintf(stderr,"\n\nERROR writing to socket");
                 //WaitACK(sockfd);
                                
                int counter = 0;
                while(1){
                    bzero(buffer,256);
                    n = read(sockfd,buffer,255);
                    if (n < 0) fprintf(stderr,"\n\nERROR reading from socket");
                    if(strcmp(buffer,"\0") != 0){
                        //printf("User: %s, selected event %d, number of seats %d\n",User, event, seats);
                        printf("User: %s\n", buffer); 
                        counter++;
                    }   
                    else
                        break; 
                }
            }
        } 

    }


}
//===================================================================
//===================================================================

