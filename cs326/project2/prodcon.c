/*
#Filename: prodcon.cpp

#Name: Seth Roller

#Date: 10/08/19

#Program Name: prodcon.cpp

#Assignment Number: 2

#Problem:
Will be outputting 1 to 1000 through
shared memory and semaphores

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
#include <wait.h>

#define SEMPERM 0600
#define SHMPERM 0600

void p(int semid, int who) {
  
  struct sembuf p_buf;
  
  p_buf.sem_num = 0;
  p_buf.sem_op = -1;
  p_buf.sem_flg = 0;
 
  if (semop(semid,&p_buf,1) < 0) {
    printf("fatal p error on semaphore %d %d\n",semid,semop(semid,&p_buf, 1));
    fflush(stdout);
  }

}

void v(int semid, int who) {

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
  }semnum, semnum1;

  //declare 2 
  semnum.val = 10;
  semnum1.val = 0;
  
  int segid;
  
  int sem_id;
  int sem_id1;
  int sem_id2;
  
  int statloc;
  int retval;
  int receive;
  int* index;
  
  int *buf;

   
  //assign the memory with buffer size of 10
  segid= shmget (IPC_PRIVATE, sizeof(int)*10,IPC_CREAT|IPC_EXCL|SHMPERM);

  if (segid == -1) {
    perror("Assigning Memory Failed:\n");
    return(-1);
  }
  
  //assign the 2 sem_ids, one for consumption and producing
  sem_id = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
  sem_id1 = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
  
  //attach them to the semnum
  receive = semctl(sem_id,0,SETVAL, semnum);

  if (receive == -1) {
    perror("Assigning Semaphore 1 Failed:\n");
    return(-1);
  }
  
  receive = semctl(sem_id1,0,SETVAL, semnum1);

  if (receive == -1) {
    perror("Assigning Semaphore 2 Failed:\n");
    return(-1);
  }

  retval = fork();
   
  if (retval<0)
   {
     perror("Fork Failed:");
     return(-1); /*fork failed*/
   }

  //child
 if(retval==0) {
   int sum = 0;
   printf("child's pid: %d\n", getppid());
   fflush(stdout);
   
   buf = (int *)shmat (segid, (int *)0,0);


   for (int i = 0; i < 1000; i++) {
     p(sem_id1, 0);
     int temp = buf[i % 10];
     v(sem_id, 0);
     //printf("buf after consuming: %d\n", temp);     
     fflush(stdout);
     sum += temp;
   }

   printf("sum of all items: %d\nchild is terminating\n", sum);
   fflush(stdout);
 }

 else {
   
   printf("\nName: Seth Roller\n");
   printf("Lab Name: Single Consumer/Single Producer\n");
   printf("Problem: Will be printing out the first 1000\n");
   printf("integers through the theory\n");
   printf("of a single producersingle consumer by memory and semaphores.\n\n");
   
   printf("parent's pid: %d\n", getppid());
   fflush(stdout);
   
   buf = (int *)shmat (segid, (int *)0,0);

   
   for (int i = 0; i < 1000; i++) {   
     p(sem_id, 1);
     buf[i % 10] = i+1;
     v(sem_id1, 1);
   }

   wait(&statloc);

   semctl(sem_id,0,IPC_RMID,NULL);
   semctl(sem_id1,0,IPC_RMID,NULL);
   shmctl(segid,IPC_RMID,NULL);
   printf("parent is terminating\n\n");
   fflush(stdout);
 }

 
 
}
