#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define READ 1

#define WRITTEN 2

int data;

int flag=READ;

pthread_mutex_t lockit=PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t signalit=PTHREAD_COND_INITIALIZER;

void *produce(void *thread_number)

{

int i;

printf("I am the producer and my thread number is %ld \n",

(long) thread_number);

for (i=1; i<=10;i++)

{

pthread_mutex_lock(&lockit);

while (flag !=READ)

{

pthread_cond_wait(&signalit,&lockit);

}

data=i;

flag=WRITTEN;

pthread_cond_broadcast(&signalit);

pthread_mutex_unlock(&lockit);

}/* end for*/

printf("producer exiting\n");

return(NULL);

}//end of producer

void *consume(void *thread_number)

{

int i;

printf("I am the consumer and my thread number is %ld \n",

(long)thread_number);

for (i=1;i<=10;i++)

{

pthread_mutex_lock(&lockit);

while (flag != WRITTEN)

{

pthread_cond_wait(&signalit,&lockit);

}

printf("the data is: %i\n",data);

flag=READ;

pthread_cond_broadcast(&signalit);

pthread_mutex_unlock(&lockit);

}/* end for*/

printf("consumer exiting\n");

return(NULL);

}

int main()

{

void *statusp;

pthread_t *threads;

if((threads=(pthread_t *)malloc(sizeof(pthread_t)*2))==NULL)

{

printf("error allocating\n");

exit(-1);

}

if((pthread_create(&threads[0],NULL,produce,(void *)0))!=0)

{

printf("error creating producer\n");

exit(-1);

}

if(pthread_create(&threads[1],NULL,consume,(void *)1)!=0)

{

printf("error creating consumer\n");

}

if((pthread_join(threads[0],&statusp))!=0)

{

printf("couldn't join with producer\n");

}

if((pthread_join(threads[1],&statusp))!=0)

{

printf("couldn't join with consumer\n");

}

if((pthread_mutex_destroy(&lockit))!=0)

{

printf("mutex destroy failed\n");

}

if((pthread_cond_destroy(&signalit))!=0)

{

printf("cond destroy failed\n");

}

}
