/*
#Filename: wrestling.c

#Name: Seth Roller

#Date: 11/07/19

#Program Name: wrestling.c

#Assignment Number: 4

#Problem:
Implemeting the wrestler/patron situations

#Flow:
Above is the wait and signal functions
for the semaphores. Following that is the main
function that contains the fork, with the child 
and parent statements

#Sources: p and v functions were from your website
and the child and parent sections were based off 
of fig. 5-13 in the textbook. Based off of the barber
shop code that was handed out in class

 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/errno.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
#include <stdlib.h>

#define SEMPERM 0600
#define SHMPERM 0600
#define MAX 20
#define ADMIT 3

#define REMAIN 0
#define STATUS 1
#define INROOM 2
#define WWIN 3
#define PWIN 4

/*
the wait command
 */
void p(int semid) {
  
  struct sembuf p_buf;
  
  p_buf.sem_num = 0;
  p_buf.sem_op = -1;
  p_buf.sem_flg = 0;
 
  if (semop(semid,&p_buf,1) < 0) {
    printf("fatal p error on semaphore %d %d\n",semid,semop(semid,&p_buf, 1));
    fflush(stdout);
  }

}

/*
the signal command
 */
void v(int semid) {

  struct sembuf v_buf;
  
  v_buf.sem_num = 0;
  v_buf.sem_op = 1;
  v_buf.sem_flg = 0;
 
  if(semop(semid,&v_buf, 1) < 0) {
    printf("fatal v error on semaphore %d %d\n",semid, semop(semid,&v_buf, 1));
    fflush(stdout);
  }
  
}



int main(int argc, char *argv[]) {

  union {
    int val;
    struct semid_ds *buffer;
    ushort *array;
  }WakeU, AdmitU, csU, mutexU;

  //declare unions
  WakeU.val = 0;
  AdmitU.val = 0; 
  csU.val = 0;
  mutexU.val = 1;

  int segid;

  //sem ints
  int Wake;
  int Admit;
  int cs;
  int mutex;

  int statloc;
  int retval;
  int receive;
  
  //0 is the matchesRemain, 1 is the status, 
  //2 is the inRoom, 3 is for wrestler W,
  //4 is for the patrons W
  int* buf;
 
  //assign the memory 
  segid= shmget (IPC_PRIVATE, sizeof(int)*5,IPC_CREAT|IPC_EXCL|SHMPERM);

  if (segid == -1) {
    perror("Assigning Memory Failed:\n");
    return(-1);
  }


  //assign the 2 sem_ids, one for consumption and producing
  Wake = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
  Admit = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
  cs = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
  mutex = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
  
  //attach the semaphores
  
  receive = semctl(Wake,0,SETVAL, WakeU);
  if (receive == -1) {
    perror("Assigning Semaphore 1 Failed:\n");
    return(-1);
  }

  receive = semctl(Admit,0,SETVAL, AdmitU);
  if (receive == -1) {
    perror("Assigning Semaphore 2 Failed:\n");
    return(-1);
  }

  receive = semctl(cs,0,SETVAL, csU);
  if (receive == -1) {
    perror("Assigning Semaphore 3 Failed:\n");
    return(-1);
  }

  receive = semctl(mutex,0,SETVAL, mutexU);
  if (receive == -1) {
    perror("Assigning Semaphore 5 Failed:\n");
    return(-1);
  }

  //set the seed
  srand(getpid());

  //first fork to set the parent and arm
  //wrestler
  retval = fork();
  
  if (retval<0) {
      perror("Fork Failed:");
      return(-1); /*fork failed*/
    }

  //child
  if(retval==0) {

    printf("arm wrestler's pid: %d\n", getppid());
    fflush(stdout);

    //attach memory
    buf = (int *)shmat (segid, (int *)0,0);

    //while loop for arm wrestler
    while (true) {
      //wait on a patron
      p(Wake);
      //mutex to check data
      p(mutex);

      //this will hit once a patron
      //wants to wrestle past his limit
      //he will check and set status
      //to closed and then terminate
      if (buf[REMAIN] >= MAX) {
	buf[STATUS] = 1;
	v(mutex);
	break;
      }
      
      buf[INROOM]--;
      v(Admit);
      printf("admitted a patron, seat freed up\n");
      fflush(stdout);

      v(mutex);

      printf("arm wrestler is wrestling\n");
      fflush(stdout);
      
      //wait to synchronize with patron
      p(cs);

      //this doesnt have to be in mutex
      //dr bracken informed me since the wrestler
      //only accesses the winnings and remains
      //and the patrons dont, mutex isnt necessary
      //when editing these variables
      //wrestle results
      if (rand() % 4 == 0) {
	buf[REMAIN]++;
	buf[PWIN]++;
	printf("RESULT: PATRON WINS MATCH\n\n");
	fflush(stdout);
      }
      
      else {	
	buf[REMAIN]++;
	buf[WWIN]++;
	printf("RESULT: ARM WRESTLER WINS MATCH\n\n");
	fflush(stdout);
      }


      //end of CS     
    }
    
    printf("wrestler is terminating\nwaking everyone up\n\n");
    fflush(stdout);

    //wake everyone up
    v(Admit);
    
    
  }

  //this is the parent
  else {    

    //print out description
    printf("\nName: Seth Roller\n");
    fflush(stdout);
    printf("Lab Name: Arm Wrestler\n");
    fflush(stdout);
    printf("Problem: Arm Wrestler is in a tent and\n");
    fflush(stdout);
    printf("has to arm wrestle patrons who wait in\n");
    fflush(stdout);
    printf("a waiting room with a limited capacity.\n\n");
    fflush(stdout);
    
    printf("parent's pid: %d\n", retval);
    fflush(stdout);

    
    for (int i = 0; i < 5; i++) {

      //fork 5 children
      retval = fork();

      if (retval<0) {
	perror("Fork Failed:");
	return(-1); /*fork failed*/
      }

      /*
	patron section of code
       */
      else if (retval == 0) {

	//attach memory
	buf = (int *)shmat (segid, (int *)0,0);
      
	printf("Patron %d entered the fair\n", getpid());
	fflush(stdout);
	
	while (true) {
	  
	  //wait for mutual exclusion
	  //check if room is filled else return mutex
	  p(mutex);

	  if (buf[INROOM] < ADMIT && buf[STATUS] != 1) {
	    //inc in the room
	    buf[INROOM]++;
	    
	    printf("Patron %d is now in the waiting room, he's #%d\n",
		   getpid(),
		   buf[INROOM]);
	    fflush(stdout);
	    v(Wake);
	    v(mutex);
	    //then wake up wrestler since hes in room

	    //wait to be admitted
	    p(Admit);
	    //check if the status changed
	    p(mutex);
	    if (buf[STATUS] == 1) {
	      v(Admit);
              v(mutex);
              break;
	     
	    }

	    //signal to arm wrestler to wrestle
	    v(mutex);
	    v(cs);
            printf("Patron %d is wrestling\n", getpid());
            fflush(stdout);
	  }

	  //if status is closed exit
	  else if (buf[STATUS] == 1) {
	    v(mutex);
	    break;
	  }

	  //if the room is full
	  else {
	    printf("Patron %d finds room rull, leaves\n", getpid());
	    fflush(stdout);
	    v(mutex);
	  }
	  
	}
	//if finishes break so it wont continue to fork
	break;
      } // end of if for patron

      
    }
    
    //if parent wait
    if (retval != 0) {

      buf = (int *)shmat (segid, (int *)0,0);
      
      for (int i = 0; i < 6; i++) {
	wait(&statloc);
	printf("process exiting\n");
	fflush(stdout);
      }

       printf("Arm Wrestler's Winning is $%d\n", buf[WWIN]*10);
       fflush(stdout);
       printf("Patron's Winnings are $%d\n", buf[PWIN]*25);

       semctl(Wake,0,IPC_RMID,NULL);
      semctl(Admit,0,IPC_RMID,NULL);
      semctl(cs,0,IPC_RMID,NULL);
      semctl(mutex,0,IPC_RMID,NULL);
      shmctl(segid,IPC_RMID,NULL);
      
    }//end of if
         
    
  }//end of parent
      
}//end of main

