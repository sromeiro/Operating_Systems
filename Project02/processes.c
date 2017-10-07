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
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* key number */
#define SHMKEY ((key_t) 1497)

// semaphore key
 #define SEMKEY ((key_t) 400L)
 // number of semaphores being created
 #define NSEMS 1

 /* change the key number */
 #define SHMKEY ((key_t) 7777)

typedef struct
{
  int value;
} shared_mem;

shared_mem *total;

int sem_id;// semaphore id

// semaphore buffers
static struct sembuf OP = {0,-1,0};
static struct sembuf OV = {0,1,0};
struct sembuf *P =&OP;
struct sembuf *V =&OV;

// semapore union used to generate semaphore
typedef union
{
	int val;
	struct semid_ds *buf;
	ushort *array;
} semunion;


/*******************************FUNCTIONS**************************************/

void process1();
void process2();
void process3();
void process4();
int POP();
int VOP();

/******************************************************************************/

int main()
{
  int shmid, pid1, pid2, pid3, pid4, ID, status;
  int   value, value1;
  int semnum = 0;
	semunion semctl_arg;
  semctl_arg.val = 1;

  char *shmadd;
  shmadd = (char *) 0;

  /* Create semaphores */
	sem_id = semget(SEMKEY, NSEMS, IPC_CREAT | 0666);
	if(sem_id < 0) printf("Error in creating the semaphore./n");

	/* Initialize semaphore */
	value1 =semctl(sem_id, semnum, SETVAL, semctl_arg);
	value =semctl(sem_id, semnum, GETVAL, semctl_arg);
	if (value < 1) printf("Eror detected in SETVAL.\n");


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
    printf("\n"); //Just for formatting purposes
    process1();
    exit(0);
  }

  //Create process fork and run process2 only if it's child process
  if (pid1 != 0 && (pid2 = fork()) == 0)
  {
    process2();
    exit(0);
  }

  //Create process fork and run process3 only if it's child process
  if (pid1 != 0 && pid2 != 0 && (pid3 = fork()) == 0)
  {
    process3();
    exit(0);
  }

  //Create process fork and run process4 only if it's child process
  if (pid1 != 0 && pid2 !=0 && pid3 != 0 && (pid4 = fork()) == 0)
  {
    process4();
    exit(0);
  }

  //Waits for each process to end and prints the correct one
  pid1 = wait(&status);
  pid2 = wait(&status);
  pid3 = wait(&status);
  pid4 = wait(&status);

  if(pid1 > 0 && pid2 > 0 && pid3 > 0 && pid4 > 0)
  {
    printf("Child process %d has ended\n", pid1);
    printf("Child process %d has ended\n", pid2);
    printf("Child process %d has ended\n", pid3);
    printf("Child process %d has ended\n", pid4);
  }

  //Detach the memory
  if (shmdt(total) == -1)
    {
      perror ("shmdt");
      exit (-1);
    }
  //Removes the shared memory
  shmctl(shmid, IPC_RMID, NULL);

  /* De-allocate semaphore */
  semctl_arg.val = 0;
  status =semctl(sem_id, 0, IPC_RMID, semctl_arg);
  if( status < 0) printf("Error in removing the semaphore.\n");

  printf("   \nSimulation has ended\n\n");
  //Successful program termination
  return 0;
}

/***************************FUNCTION DEFINITIONS*******************************/

void process1()
{
  int i;
  for(i = 0; i < 100000; i++)
  {
    POP(); //Wait for other processes in the critical section before entering
    total->value++;
    VOP(); //Release your turn from the critical section so others can enter
  }
  printf("Value is %d\n", total->value);
}

void process2()
{
  int i;

  for(i = 0; i < 200000; i++)
  {
    POP(); //Wait for other processes in the critical section before entering
    total->value++;
    VOP(); //Release your turn from the critical section so others can enter
  }
  printf("Value is %d\n", total->value);
}

void process3()
{
  int i;
  for(i = 0; i < 300000; i++)
  {
    POP(); //Wait for other processes in the critical section before entering
    total->value++;
    VOP(); //Release your turn from the critical section so others can enter
  }
  printf("Value is %d\n", total->value);
}

void process4()
{
  int i;
  for(i = 0; i < 500000; i++)
  {
    POP(); //Wait for other processes in the critical section before entering
    total->value++;
    VOP(); //Release your turn from the critical section so others can enter
  }
  printf("Value is %d\n", total->value);
}

// POP (wait()) function for semaphore to protect critical section
int POP()
{
	int status;
	status = semop(sem_id, P,1);
	return status;
}

// VOP (signal()) function for semaphore to release protection
int VOP()
{
	int status;
	status = semop(sem_id, V,1);
	return status;
}
