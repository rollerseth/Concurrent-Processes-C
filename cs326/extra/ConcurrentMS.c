/*
#Filename: ConcurrentMS.c

#Name: Seth Roller

#Date: 12/05/19

#Program Name: ConcurrentMS.c

#Assignment Number: Extra

#Problem:

#Flow:


#Sources: 


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
#include <math.h>

#define SHMPERM 0600



bool isPowerOfTwo(int n) 
{ 
  if (n == 0) 
    return 0;
  
  while (n != 1) { 
    if (n%2 != 0) 
      return 0; 
    n = n/2; 
  }
  
  return 1; 
} 


void Merge(int *S, int index, int n) {

  int i, j, k;
  index = index-1;
  int m = (n-1-index)/2;
  
  int l = m-index+1;
  int o =n-1-m;
  
  int left[l+3];
  int right[o+3];

  for (i = 0;i < l; i++) {
    left[i] = S[index+i];
  }

  for (i = 0; i< o; i++) {
    right[i] = S[m+i+1];
  }

  i=0;
  j=0;
  k=index;
  
  while (i<l && j<o){


    if (left[i]<right[j]) {
      S[k] = left[i];
      k++;
      i++;
    }

    else {
      S[k] = left[i];
      k++;
      i++;
    }

    while (i < l) {
      S[k] = left[i];
      k++;
      i++;
    }

    while (j < o) {
      S[k] = right[j];
      k++;
      j++;
    }
  }
}


void MergeSort(int *S, int index,int n) {

  if (n > 1) {
    int retval1;
    int retval2;
    
    int statloc;

    retval1 = fork();
    if (retval1<0) {
      perror("Fork Failed:");
      exit(-1);
    }
    
    //first child
    else if (retval1 == 0) {
      MergeSort(S,index, n/2);
      exit(0);
    }
    
    //parent
    else {
      
      retval2 = fork();
      if (retval2<0) {
	perror("Fork Failed:");
        
      }
      
      //2nd child
      else if (retval2 == 0) {
	MergeSort(S, (index+n)/2, n/2);
        exit(0);
      }
      
      
    }//end of paren
	
    wait(&statloc);
    wait(&statloc);
    printf("%d %d\n", n ,index);
    fflush(stdout);
    Merge(S, index, n);
	//merge the two arrays
       	
  }
  
}//end of n > 1



int main(int argc, char *argv[]) {

  int segid;

  //sem ints

  int n;
  int *buf;

  printf("Name: Seth Roller\n");
  printf("Lab: Distributed Mergesort\n\n");

  while (true) {
    printf("Enter n: ");
    scanf("%d", &n);
    printf("You entered %d\n", n);
    if (isPowerOfTwo(n))
      break;
    printf("Enter a number that is power of 2\n");
  }
  
  //assign the memory 
  segid= shmget (IPC_PRIVATE, sizeof(int)*n,IPC_CREAT|IPC_EXCL|SHMPERM);

  if (segid == -1) {
    perror("Assigning Memory Failed:\n");
    return(-1);
  }

  buf = (int *)shmat (segid, (int *)0,0);

  for (int i = 0; i < n; i++) {
    printf("Enter data #%d: ", i);
    scanf("%d", &buf[i]);
    printf("You entered %d\n", buf[i]);
  }

  MergeSort(buf, 0, n);


  printf("sorted array\n");
  for (int i = 0; i < n; i++) {
    printf("%d\n", buf[i]);
  }
  
  shmctl(segid,IPC_RMID,NULL);
      
      
}//end of main

