/*
#Filename: safesigex.c

#Name: Seth Roller

#Date: 11/26/19

#Program Name: safesigex.c

#Assignment Number: 6.1

#Problem:
Using signals to execute the 2nd child from
the parent and then the 1st child from the 
2nd child

#Flow:
Function declarations followed by main, which contains
a for loop with a fork. Then is the child function and the 
sig_user function.

#Sources: Source code from Dr. Brackens
website that was used as a base.

 */

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>



void sig_user (int signo);
void child();


/* please note the use of global variables. Global variables are 
 * very dangerous and should only be used when absolutely necessary.
 * this simple example does not justify the use of global variables.
 * It was, however, taken from a much more complicated program, 
 * complicated enough to justify the use of global variables.
 */
static sigset_t newmask, oldmask, zeromask;

//set up array for next deliverable
int pids[1];

int main (void)
{

     

  int pid, status, retval;
  pid = 0;
   
  /*  stuff for blocking signals */

  if (signal(SIGUSR1, sig_user) == SIG_ERR)
    {
      printf("error creating signal handler \n");
      fflush(stdout);
    }
  sigemptyset(&zeromask);
  sigemptyset(&newmask);
  sigaddset(&newmask,SIGUSR1);
  if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    {
      printf("error in sigprocmask \n");
      fflush(stdout);
    }

  for (int i = 0; i < 2; i++) {
    //this will only occur once since i set pid
    //to 0 initially
    if (pid != 0)
      pids[0] = pid;
    
    pid = fork(); 
  if (pid < 0)
    {
      printf("fork failed \n");
      fflush(stdout);
      exit (-1);
    }/*end if pid < 0 */
  else
    {
      if (pid == 0)
        {
	  
	  child(i);
	  exit(0); /* child must exit so as to not execute 
		      parent stuff */
        }/* end if pid == 0 */
    }/* end else pid < 0*/   
     
	  
  }	  

  printf("parent signalling child %d\n", pid);
  fflush(stdout);
  kill(pid, SIGUSR1);
  
  if (retval < 0) {
      printf("signal error \n");
      fflush(stdout);
    }

  printf("parent waiting for child to terminate\n");
  fflush(stdout);
  
  if(pid=wait( &status)<0)
    {
      printf("waitpid error\n");
      fflush(stdout);
    } 
	
  printf("parent terminating \n");
  fflush(stdout);
} /* end of main*/


void child(int b)
{

  int retval;
  
  printf("child %d has control  \n", getpid());
  fflush(stdout);
   
  /* parent created  signal handler */

  
   
  //wait for the signal

   
  sigsuspend(&zeromask); /*wait */

   
  if (sigprocmask(SIG_SETMASK, &oldmask, NULL)<0)
    {
      printf("sigmask error \n");
      fflush(stdout);
    }

  //if its the 2nd child we want to signal the first
  if (b == 1) {
    printf("signaling child %d from child %d\n", pids[0], getpid());
    fflush(stdout);
    
    retval = kill(pids[0],SIGUSR1);
    
    if (retval < 0) {
      printf("signal error\n");
      fflush(stdout);
    }
  }

  
  printf("child %d terminating after signal  \n", getpid());
  fflush(stdout);

}
void sig_user (int signo)
{
  int sing, index;
  if (signo == SIGUSR1)
    {

      printf("signal 1 received by signal handler child %d \n", getpid());

      fflush(stdout);



    }/* end if */
  
}/* end of signal handler */
