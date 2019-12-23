/*
#Filename: prodconth.cpp

#Name: Seth Roller

#Date: 10/31/19

#Program Name: prodconth.cpp

#Assignment Number: 3

#Problem:
Will be outputting 1 to 99 through
mutex, conditional variables and threads

#Flow:
The produce and consume functions are first.
Followed by is main, where most of the thread
checking is performed. 

#Sources: 

 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<stdbool.h>

#define READ 1
#define WRITTEN 2

//global variables
int keepTrack = 0;
int shared = 0;
int data[10];
int flag = READ;
pthread_mutex_t lockit1=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t signalit=PTHREAD_COND_INITIALIZER;

void *produce(void *thread_num) {
  
  printf("I am the producer and my thread number is %ld \n",(long) thread_num);
  fflush(stdout);
  
  while (true) {

    pthread_mutex_lock(&lockit1);
    if (shared > 99) {
      pthread_cond_broadcast(&signalit);
      pthread_mutex_unlock(&lockit1);
      break;
    }
    while (flag != READ && shared <= 99) 
      pthread_cond_wait(&signalit,&lockit1);

    if (shared > 99) {
        pthread_cond_broadcast(&signalit);
        pthread_mutex_unlock(&lockit1);
        break;
      }
    
    data[shared % 10]=shared;
  
    printf("producer data[%d] = %d\nP:hreadnum: %ld\n\n", shared%10, shared,
	   (long)thread_num);
  
    fflush(stdout);
    flag=WRITTEN;
    shared++;
    pthread_cond_broadcast(&signalit);    
    pthread_mutex_unlock(&lockit1);

  }/* end for*/
  
  printf("producer %ld exiting\n", (long) thread_num);
  fflush(stdout);
  
  return(NULL);
  
}

void *consume(void *thread_num) {

  printf("I am the consumer and my thread number is %ld \n",(long)thread_num);
  fflush(stdout);
  
  while (true) {

    pthread_mutex_lock(&lockit1);

    if (keepTrack > 99) {
        pthread_cond_broadcast(&signalit);
        pthread_mutex_unlock(&lockit1);
        break;
      }
    while (flag != WRITTEN && keepTrack <= 99) 
      pthread_cond_wait(&signalit,&lockit1);

    if (keepTrack > 99) {
        pthread_cond_broadcast(&signalit);
        pthread_mutex_unlock(&lockit1);
        break;
      }
    printf("consumer data[%d] = %d\nC:threadnum: %ld\n", keepTrack%10, data[keepTrack%10],(long)thread_num);
   
    fflush(stdout);
    
    flag=READ;
    keepTrack++;
    pthread_cond_broadcast(&signalit);
    pthread_mutex_unlock(&lockit1);
    
  }/* end for*/

  printf("consumer %ld exiting\n", (long)thread_num);
  fflush(stdout);
  
  return(NULL);
 
  
}


int main(int argc, char *argv[]) {

  void *statusp;

  pthread_t *threads;

  if ((threads=(pthread_t*)malloc(sizeof(pthread_t)*4)) == NULL) {
    printf("error allocating\n");
    fflush(stdout);
    exit(-1);
  }

  
  if((pthread_create(&threads[0],NULL,produce,(void *)0))!=0) {
    
    printf("error creating 1st producer\n");
    fflush(stdout);
    //exit(-1);      
  }

  
  if((pthread_create(&threads[1],NULL,produce,(void *)1))!=0) {
    
    printf("error creating 2nd producer\n");
    fflush(stdout);
    exit(-1);      
  }

  if(pthread_create(&threads[2],NULL,consume,(void *)2)!=0) {

    printf("error creating 1st consumer\n");
    fflush(stdout);
    exit(-1);
  }

  if(pthread_create(&threads[3],NULL,consume,(void *)3)!=0) {

    printf("error creating 1st consumer\n");
    fflush(stdout);
    exit(-1);
  }

  if((pthread_join(threads[0],&statusp))!=0) {

    printf("couldn't join with 1st producer\n");
    fflush(stdout);
    exit(-1);
  }

  if((pthread_join(threads[1],&statusp))!=0) {
      
    printf("couldn't join with 2nd producer\n");
    fflush(stdout);
    exit(-1);  
  }

  if((pthread_join(threads[2],&statusp))!=0) {

    printf("couldn't join with 1st consumer\n");
    fflush(stdout);
    exit(-1);
  }

  if((pthread_join(threads[3],&statusp))!=0) {
      
    printf("couldn't join with 2nd consumer\n");
    fflush(stdout);
    exit(-1);  
  }
  
  if((pthread_mutex_destroy(&lockit1))!=0) {
      
    printf("mutex 1 destroy failed\n");
    fflush(stdout);
    exit(-1);     
  }
  
  if((pthread_cond_destroy(&signalit))!=0) {
 
    printf("cond destroy failed\n");
    fflush(stdout);
    exit(-1);
  }
  
}
