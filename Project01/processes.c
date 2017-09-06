/*
* COP4600
* Project01
* Esthevan Romeiro
* U16803837
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* key number */
#define SHMKEY ((key_t) 1497)

typedef struct
{
  int value;
} shared_mem;

shared_mem *total;

/*******************************FUNCTIONS**************************************/

void process1();
void process2();
void process3();
void process4();

/******************************************************************************/

int main()
{
  int shmid, pid1 = 100, pid2 = 100, pid3 = 100, pid4 = 100, ID, status;
  char *shmadd;
  shmadd = (char *) 0;

  if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
  {
    perror ("shmget");
    exit (1);
  }
  if ((total = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1)
  {
    perror ("shmat");
    exit (0);
  }

  total->value = 0;
/*
  if ((pid1 = fork()) == 0)
  {
    process1();
  }
*/

  //printf("Before fork my pid is %d and pid1 is %d\n", getpid(), pid1);
  pid1 = fork();
  //printf("After fork my pid is %d and pid1 is %d\n", getpid(), pid1);

  if(pid1 == 0)
  {
    //We are the child process
    printf("I am the child process from pid1 with ID %d\n", getpid());
    printf("Processing...\n");
    process1();
  }
  else
  {
    //We are the parent process
    printf("I am the parent process from pid1 with ID %d\n", getpid());
    printf("Waiting on child...\n");
    wait(NULL);
    printf("Child process from pid1 has finished\n");
  }

/*
  if(pid2 == 0)
  {
    //We are the child process
    printf("I am the child process from pid2 with ID %d\n", getpid());
    printf("Processing...\n");
    process2();
  }
  else if(pid2 != 100)
  {
    //We are the parent process
    printf("I am the parent process from pid2 with ID %d\n", getpid());
    printf("Waiting on child...\n");
    wait(NULL);
    printf("Child process from pid2 has finished\n");
    pid3 = fork(); //Create a new fork with pid3
  }

  if(pid3 == 0)
  {
    //We are the child process
    printf("I am the child process from pid3 with ID %d\n", getpid());
    printf("Processing...\n");
    process3();
  }
  else if(pid3 != 100)
  {
    //We are the parent process
    printf("I am the parent process from pid3 with ID %d\n", getpid());
    printf("Waiting on child...\n");
    wait(NULL);
    printf("Child process from pid3 has finished\n");
    pid4 = fork(); //Create a new fork with pid4
  }

  if(pid4 == 0)
  {
    //We are the child process
    printf("I am the child process from pid4 with ID %d\n", getpid());
    printf("Processing...\n");
    process4();
  }
  else if(pid4 != 100)
  {
    //We are the parent process
    printf("I am the parent process from pid4 with ID %d\n", getpid());
    printf("Waiting on child...\n");
    wait(NULL);
    printf("Child process from pid4 has finished\n");
  }
*/




  printf("\nProgram terminated\n\n");
  //Successful program termination
  return 0;
}

/***************************FUNCTION DEFINITIONS*******************************/

void process1()
{
  printf("Process 1 running\n");
  while(total->value < 100000)
    total->value += 1;
  //sleep(5);
  printf("Value is %d\n", total->value);
}

void process2()
{
  printf("Process 2 running\n");
  while(total->value < 200000)
    total->value += 1;
  //sleep(5);
  printf("Value is %d\n", total->value);
}

void process3()
{
  printf("Process 3 running\n");
  while(total->value < 300000)
    total->value += 1;
  //sleep(5);
  printf("Value is %d\n", total->value);
}

void process4()
{
  printf("Process 4 running\n");
  while(total->value < 500000)
    total->value += 1;
  //sleep(5);
  printf("Value is %d\n", total->value);
}
