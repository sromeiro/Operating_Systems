/*
* COP4600
* Project03
* Esthevan Romeiro
* U16803837
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#define _REENTRANT
/* key number */
#define SHMKEY ((key_t) 1497)

// semaphore key
#define SEMKEY ((key_t) 400L)

// number of semaphores being created
#define NSEMS 1

#define BUFFER_SIZE 15

typedef struct
{
  int value;
} shared_mem;

//Sempaphores for mutex, full and empty
sem_t mutex, full, empty;

char stuff; //Stuff we read in
FILE* fp;

shared_mem *count;

char buffer[BUFFER_SIZE]; //Buffer that holds stuff

bool flag = true; //Flag that signals the end
int producerIndex, consumerIndex, producerLoop, consumerLoop;

//============================PROTOTYPES======================================//
void* thread1 (void *ptr); //Producer thread
void* thread2 (void *ptr); //Consumer thread

//===============================MAIN=========================================//

int main()
{
  int shmid;
  char *shmadd;
  shmadd = (char *) 0;

  pthread_t	tid1[1];           /* process id for thread 1 */
  pthread_t	tid2[1];           /* process id for thread 2 */
  pthread_attr_t	attr[1];     /* attribute pointer array */
  sem_init(&mutex,0,1);
  sem_init(&full,0,0);
  sem_init(&empty,0,BUFFER_SIZE);

  if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
  {
    perror ("shmget");
    exit (1);
  }
  if ((count = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1)
  {
    perror ("shmat");
    exit (0);
  }

  fflush(stdout);
  /* Required to schedule thread independently.*/
  pthread_attr_init(&attr[0]);
  pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
	/* end to schedule thread independently */

  /* Create the threads */
  pthread_create(&tid1[0], &attr[0], thread1, NULL);
  pthread_create(&tid2[0], &attr[0], thread2, NULL);

  /* Wait for the threads to finish */
  pthread_join(tid1[0], NULL);
  pthread_join(tid2[0], NULL);

  //destroy semaphores
  sem_destroy(&empty);
  sem_destroy(&full);
  sem_destroy(&mutex);

  //Detach the memory
  if (shmdt(count) == -1)
    {
      perror ("shmdt");
      exit (-1);
    }

  //Removes the shared memory
  shmctl(shmid, IPC_RMID, NULL);


  printf("   \nSimulation has ended\n\n");

  //Terminate threads
  pthread_exit(NULL);
  
  return 0;
}

//================================DEFINITIONS=================================//

void* thread1 (void *ptr)
{
  count->value = 0;
  int bufferItems = 0; //Needed to know how many items currently in the buffer
  producerIndex = 0; //Needed to know the index of Producer
  producerLoop = 0; //Needed to know how many loops producer has gone through

  fp = fopen("mytest.dat", "r");
  while (fscanf(fp, "%c", &stuff) != EOF && stuff != '\n' && count->value <= 150)
  {
    //Perform checks first
    if(count->value % BUFFER_SIZE == 0 && count->value > 0)
    {
      producerLoop++; //Producer has looped through the buffer. Increment.
    }

    if(bufferItems == BUFFER_SIZE)
    {
      sem_post(&full); //Buffer is full, increment full so Thread2 can run.
      bufferItems--; //Decrement bufferItems since Thread2 will consume
    }

    //Checks failed.
    sem_wait(&empty); //Wait for an empty slot before producing
    sem_wait(&mutex); //Wait for token to become free to use. Use token if available
    producerIndex = count->value % BUFFER_SIZE;
    buffer[producerIndex] = stuff;
    count->value++;
    bufferItems++;
    printf("PRODUCED -> %c\n", stuff);
    sem_post(&mutex); //Release token so another thread can use it.
  }

  if(count->value > 150)
  {
    printf("\n\t**** CHARACTER LIMIT EXCEEDED ****\n");
  }

  producerIndex = count->value %BUFFER_SIZE; //Update index to correct value.
  close(fp); //close the file

}

void* thread2 (void *ptr)
{
  char consumed; //Stuff to consume
  consumerIndex = 0; //Needed to know the index of Consumer
  consumerLoop = 0; //Needed to know how many loops consumer has gone through
  while (flag == true)
  {
    if(consumerIndex == 15)
    {
      consumerLoop++; //Looped through buffer. So increment.
    }

    sem_wait(&full); //Wait for producer to increment this full semaphore
    sem_wait(&mutex); //Wait for token to become free to use. Use token if available
    sleep(1);
    consumerIndex = consumerIndex % BUFFER_SIZE;
    consumed = buffer[consumerIndex++];

    sem_post(&mutex); //Release token so another thread can use it.
    sem_post(&empty); //Empty slot available for producer to use.
    printf("\t\t CONSUMED -> %c\n", consumed);
    sem_post(&full); //Increment full when producer is no longer doing it so we can continue.

    if(consumerLoop == producerLoop && consumerIndex == producerIndex)
    {
      //Check we are on the same loop and same index before stopping.
      flag = false;
    }
  }
}
