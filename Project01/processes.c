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
  int shmid, pid1, pid2, pid3, pid4, ID, status;
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

  //Create process fork and run process1 only if it's child process
  if ((pid1 = fork()) == 0)
  {
    process1();

  }

  //Create process fork and run process2 only if it's child process
  if (pid1 != 0 && (pid2 = fork()) == 0)
  {
    process2();
  }

  //Create process fork and run process3 only if it's child process
  if (pid1 != 0 && pid2 != 0 && (pid3 = fork()) == 0)
  {
    process3();
  }

  //Create process fork and run process4 only if it's child process
  if (pid1 != 0 && pid2 !=0 && pid3 != 0 && (pid4 = fork()) == 0)
  {
    process4();
  }

  //Waits for each process to end and prints the correct one
  pid1 = wait(&status);
  pid2 = wait(&status);
  pid3 = wait(&status);
  pid4 = wait(&status);

  if(pid1 > 0 && pid2 > 0 && pid3 > 0 && pid4 > 0)
  {
    printf(" Child process %d has ended\n", pid1);
    printf(" Child process %d has ended\n", pid2);
    printf(" Child process %d has ended\n", pid3);
    printf(" Child process %d has ended\n", pid4);
  }

  if (shmdt(total) == -1)
    {
      perror ("shmdt");
      exit (-1);
    }

  shmctl(shmid, IPC_RMID, NULL);

  //Successful program termination
  return 0;
}

/***************************FUNCTION DEFINITIONS*******************************/

void process1()
{
  int i;
  for(i = 0; i < 100000; i++)
    total->value++;
  printf("Value is %d\n", total->value);
}

void process2()
{
  int i;
  for(i = 0; i < 200000; i++)
    total->value++;
  printf("Value is %d\n", total->value);
}

void process3()
{
  int i;
  for(i = 0; i < 300000; i++)
    total->value++;
  printf("Value is %d\n", total->value);
}

void process4()
{
  int i;
  for(i = 0; i < 500000; i++)
    total->value++;
  printf("Value is %d\n", total->value);
}
