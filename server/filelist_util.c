#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h> 

#include "filelist_util.h"
/* Gets all data from a event based on index */
int GetEventAllFromFile(char *event_file, char *event, int max_size, int index){
    char buffer[1024];

    int fd_list = open(event_file, O_RDONLY, (S_IRUSR | S_IWUSR));
    if(fd_list < 0){
      printf("Error opening the file in GetEventAllFromFile: %s\n", strerror(errno));
      return ERROR;
    }
    char a = 0;
    int size=0;
    int number_of_events = 0;
    while(1){

      int k =0;
      do{
        size = read(fd_list,&a,1);
        if(size < 0){
          printf("Error reading the file: %s\n", strerror(errno));
          close(fd_list);
          return ERROR;
          //exit(0);
        }
        if( size <= 0)
          break;

        buffer[k] = a;
        k++;
      }while(a != ';' );

      buffer[k] = 0;

      if(size == 0)
        break;

      if(number_of_events == index){
        if(k+1 > max_size){
          close(fd_list);
          return -2;
        }
        int i;
        for(i = 0; i < k; i++)
          event[i] = buffer[i];
        event[i] = 0;
        close(fd_list);
        return i;
      }
      //printf("Found event: %s\n", buffer);
      //AddEventToList(buffer);
      number_of_events++;

     

    }
    close(fd_list);

    return EVENT_NOT_FOUND;

}

int GetEventFixedInfo(char *event_file, char *event, int max_size, int index){
  int t = GetEventAllFromFile(event_file, event,  max_size,  index);
  if(t < 0)
    return t;
  int k=0;
int counter = 0;
  while(1){

    if(event[k] == ','){
      counter++;
    }
    if(counter == 3){

      event[k] = 0;  
      return k;
    }
    k++;
    if(k == max_size)
      return EVENT_NOT_FOUND;
  }


}

/* Gets event/filename based on index */
int GetEventFromFile(char *event_file, char *event, int max_size, int index){

    int t = GetEventAllFromFile(event_file, event,  max_size,  index);
  if(t < 0)
    return t;
  int k=0;
int counter = 0;
  while(1){

    if(event[k] == ','){
      counter++;
    }
    if(counter == 1){

      event[k] = 0;  
      return k;
    }
    k++;
    if(k == max_size)
      return EVENT_NOT_FOUND;
  }



}

/* Creates a empty file with name file_name*/
int CreateFile(char *file_name){

      int fd_temp = open(file_name, O_CREAT|O_RDWR, (S_IRUSR | S_IWUSR));
      if(fd_temp < 0){
          printf("Error creating/opening the file: %s\n", strerror(errno));
          exit(0);
      }
      else{
        close(fd_temp);
      }

   /*        if(EEXIST == errno){
          printf("File %s already existed. Erasing previous file\n", file_name);
          remove(file_name);
          fd_temp = open(file_name, O_CREAT|O_RDWR, (S_IRUSR | S_IWUSR));
          if(fd_temp < 0){
            printf("Error creating/opening the file: %s\n", strerror(errno));
            exit(0);
          }
        }*/ 
}



int EventCheckMaxEntries(char *event_file, int index){
  char event[256];
  int t = GetEventAllFromFile(event_file, event,  256,  index);
  if(t < 0)
    return t;
  int k=0;
  int counter = 0;
  int number_now = 0;
  int number = 0;
  do{

    if(number_now == 1 && (event[k] >= '0' && event[k] <= '9')){
      number = number*10+(event[k]-'0');
    }
    if(event[k] == ','){
      counter++;
    }
    if(counter == 4){
      number_now = 1;  
    }
    k++;
    if(k == 256)
      return ERROR;

  }while(event[k] != ';');

  return number;

}
/* Adds username entry to event. Also checks if user already registered and returns ALREADY_REGISTERED if it is */
int EventAddRegistry(char *event_file, char *username, int n_of_seats, int index){
  char buffer[256];
  bzero(buffer,256);
  int t = GetEventFromFile(event_file, buffer, 256, index);
  if(t < 0)
    return t;

  if(EventAvailability(event_file, index)  < n_of_seats){
    return NO_SEATS;
  }
  int fd_temp = open(buffer, O_RDWR, (S_IRUSR | S_IWUSR));

    if(fd_temp < 0){
      printf("Error opening the file in EventAddRegistry: %s\n", strerror(errno));
      return -1;
    }
    char a = 0;
    int size=0;
    int number_registries = 0;
    while(1){

      int k =0;
      do{
        size = read(fd_temp,&a,1);
        if(size < 0){
          printf("Error reading the file: %s\n", strerror(errno));
          return -1;
        }
        if( size <= 0)
          break;

        buffer[k] = a;
        k++;
      }while(a != ';' );

      buffer[k] = 0;

      if(size == 0){
        bzero(buffer,256);
        sprintf(buffer, "%s,%d;\n", username, n_of_seats);
        size = write(fd_temp, buffer, strlen(buffer));
        close(fd_temp);
        if( size <= 0)
          return -1;
        return 0;
      }


      int size = strcspn(buffer, ",");
      char user[32];
      int i;
      for(i = 0; i < size && i < 32; i++)
        user[i] = buffer[i];
      user[i] = 0;

      if(strcmp(user, username) == 0){
        close(fd_temp);
        return ALREADY_REGISTERED;
      }

      

      number_registries++;

     

    }
    close(fd_temp);


}

int EventCountRegist(char *event_file, int index){

  char event_name[256];
  bzero(event_name,256);
  int t = GetEventFromFile(event_file, event_name, 256, index);
  if(t < 0)
    return t;

  int fd_temp = open(event_name, O_RDWR, (S_IRUSR | S_IWUSR));

    if(fd_temp < 0){
      printf("Error opening the file: %s\n", strerror(errno));
      return -1;
    }
    char a = 0;
    int size=0;
    int number_registries = 0;
    int number_now = 0;
    int number = 0;
    while(1){

      int k =0;
      do{
        size = read(fd_temp,&a,1);
        if(size < 0){
          printf("Error reading the file: %s\n", strerror(errno));
          return -1;
        }
        if(number_now == 1 && a != ';'){
          number = number*10+(a-'0');
        }
        if( size <= 0)
          break;
        if(a == ',')
          number_now = 1;
        k++;
      }while(a != ';' );
      number_now = 0;

      if(size == 0){
        break;
      }
      

      number_registries+=number;;
      number = 0;

     

    }
    close(fd_temp);

    return number_registries;
}


int EventAvailability(char *event_file, int index){
  return EventCheckMaxEntries(event_file, index)-EventCountRegist(event_file, index);

}


int SearchUserInFile(char *event_file, char *username, int index, char *info){


  char event_name[256];
  bzero(event_name,256);
  int t = GetEventFromFile(event_file, event_name, 256, index);
  if(t < 0)
    return t;

  int fd_temp = open(event_name, O_RDWR, (S_IRUSR | S_IWUSR));

  if(fd_temp < 0){
    printf("Error opening the file: %s\n", strerror(errno));
    return -1;
  }
  char a = 0;
  int size=0;
  int name_found = 0;
  char buffer[30];
  bzero(buffer,30);
  while(1){

    int k =0;
    do{
      size = read(fd_temp,&a,1);
      if(size < 0){
        printf("Error reading the file: %s\n", strerror(errno));
        close(fd_temp);
        return -1;
      }

      buffer[k] = a;

      if( size <= 0)
        break;
      if(a == ','){
        buffer[k] = 0;
        if(strcmp(buffer,username)){
             name_found = 1;
        }
        buffer[k] = ',';
      }
      k++;
    }while(a != ';' );

    if(name_found == 1){
      buffer[k] = 0;
      close(fd_temp);
      return 1;
    }
    


    bzero(buffer,30);
    k = 0;

    if(size == 0){
      break;
    }
    

   

  }

  close(fd_temp);
  return USER_NOT_FOUND;
}