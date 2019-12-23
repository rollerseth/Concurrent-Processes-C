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
of fig. 5-13 in the textbook

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
#define WWIN 3
#define PWIN 4
#define STATUS 1
#define REMAIN 0
#define INROOM 2

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
  AdmitU.val = 3;
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
    perror("Assigning Semaphore 4 Failed:\n");
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
      //wait on admit
      p(Wake);
      //mutex to alter data
      p(mutex);
    
      if (buf[REMAIN] >= 20) {
	buf[1] = 1;
	break;
      }
	
      buf[INROOM]--;
      v(cs);
      printf("arm wrestler is ready to wrestle match %d\n", buf[0]+1);
      fflush(stdout);
      
      //wrestle
      if (rand() % 4 == 0) {
	buf[REMAIN]++;
	buf[WWIN]++;
	printf("ARM WRESTLER WINS MATCH %d\n", buf[0]);
	fflush(stdout);
      }
      
      else {
	buf[REMAIN]++;
	buf[PWIN]++;
	printf("PATRON WINS MATCH %d\n", buf[0]);
	fflush(stdout);
      }

      printf("end of match %d\n\nthere is %d in waiting room\n",
	     buf[0], buf[2]);
      fflush(stdout);
      
      if (buf[REMAIN] >= 20) {
	buf[STATUS] = 1;
	break;
      }
      v(mutex);
      p(cs);
      //end of CS & mutex     
    }
    
    printf("wrestler is terminating\n");
    fflush(stdout);
    v(mutex);
    p(cs);
  }

  else {    

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
    
    printf("parent's pid %d\n", retval);
    fflush(stdout);

    
    for (int i = 0; i < 5; i++) {

      retval = fork();
 
      if (retval == 0) {

	//attach memory
	buf = (int *)shmat (segid, (int *)0,0);
      
	printf("\npatron %d entering\n\n", getpid());
	fflush(stdout);
	
	while (true) {
	  //wait for mutual exclusion
	  p(mutex);
	  printf("checking waiting room\n");
	  fflush(stdout);
	  if (buf[INROOM] < 3 && buf[REMAIN] < 20 &&
	      MAX-buf[REMAIN]-buf[INROOM] > 0 && buf[1] != 1) {
	    //inc in the room
	    v(mutex);//added
	    p(Admit);//added
	    printf("%d is in the waiting room\n", getpid());//added
	    fflush(stdout);//added
	    p(mutex);
	    buf[INROOM]++;
	    
	    printf("%d in the waiting room and %d is in\n", buf[INROOM], getpid());
	    fflush(stdout);
	    v(Wake);
	    v(mutex);
	    p(cs);
	    printf("%d is wrestling\n", getpid());
	    fflush(stdout);
	    v(cs);
	  }

	  //if status is closed exit
	  else if (buf[1] == 1) {
	    v(mutex);
	    break;
	  }

	  //if the room is full
	  else {
	    printf("waiting room full, Patron %d can't enter\n", getpid());
	    fflush(stdout);
	    v(mutex);
	  }
	  
	}
	//if finishes break so it wont continue to fork
	break;
      } // end of if

      
    }

    //if parent wait
    if (retval != 0) {
      
      for (int i = 0; i < 6; i++) {
	wait(&statloc);
	printf("process exiting\n");
	fflush(stdout);
      }
      
      semctl(Wake,0,IPC_RMID,NULL);
      semctl(Admit,0,IPC_RMID,NULL);
      semctl(cs,0,IPC_RMID,NULL);
      semctl(mutex,0,IPC_RMID,NULL);
      shmctl(segid,IPC_RMID,NULL);
      
    }//end of if
    
  }//end of parent
      
}//end of main
 

   


 
 

