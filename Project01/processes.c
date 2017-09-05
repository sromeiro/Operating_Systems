#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* key number */
#define SHMKEY ((key_t) 1497)

int main()
{

  printf("Program terminated\n");
  //Successful program termination
  return 0;
}
