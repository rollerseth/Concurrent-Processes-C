/*
#Filename: twelvedays.c

#Name: Seth Roller

#Date: 12/03/19

#Program Name: twelvedays.c

#Assignment Number: 6.2

#Problem:
Using signals to print out the
twelve days of christmas.


#Flow:
Function declarations followed by main, which contains
a for loop with a fork. Then is the child function and the 
sig_user function.

#Sources: Source code from Dr. Brackens
website that was used as a base.

#Complexity:
(n/6)(n+1)(n+2) is the number of gifts given
on a day

thus O(n^3)

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


const char *gifts[100]={

"Twelve Drummers Drumming",

"Eleven Pipers Piping",

"Ten Lords a Leaping",

"Nine Ladies Dancing",

"Eight Maids a Milking",

"Seven Swans a Swimming",

"Six Geese a Laying",

"Five Golden Rings",

"Four Calling Birds",

"Three French Hens",

"Two Turtle Doves",

"A Partridge in a Pear Tree"};

const char *days[25] = {

  "twelfth",
  "eleventh",
  "tenth",
  "ninth",
  "eighth",
  "seventh",
  "sixth",
  "fifth",
  "fourth",
  "third",
  "second",
  "first" };

//set up array for next deliverable
int pids[11];

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

  for (int i = 0; i < 12; i++) {
    //this will only occur once since i set pid
    //to 0 initially
    if (pid != 0)
      pids[i] = pid;
    
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

  for (int i = 0; i < 11; i++) {
    if(pid=wait( &status)<0)
      {
	printf("waitpid error\n");
	fflush(stdout);
      } 
  }
  
  printf("parent TERMINATING \n");
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

  printf("child %d printing\n", getpid());
  fflush(stdout);

  printf("On the %s day of Christmas\nmy true love sent to me:\n",
	 days[b]);
  fflush(stdout);

  if (b != 11) {
    for (int i = b; i < 11; i++) {
      printf("%s\n", gifts[i]);
      fflush(stdout);
    }
    printf("and %s\n\n", gifts[11]);
    fflush(stdout);
  }

  else {
    printf("%s\n\n", gifts[b]);
    fflush(stdout);
  }
  
  //if its not the 1st child signal the one before it
  if (b > 0) {
    printf("signaling child %d from child %d\n", pids[b], getpid());
    fflush(stdout);
    
    retval = kill(pids[b],SIGUSR1);
    
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
