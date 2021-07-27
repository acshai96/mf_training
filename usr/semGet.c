#define _GNU_SOURCE
/* Code to get the value of the semaphore */
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>

union semun 
{
	int val;
	struct semid_ds *buf;
	unsigned short  *array;
};

int main(int argc, char* argv[])
{
	int sem_id, sem_num, sem_val, ret;
	union semun sem1;

	if(argc != 3)
	{
		printf("USAGE: %s <sem_id> <sem_num>\n", program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	sem_id = atoi(argv[1]);
	sem_num = atoi(argv[2]);

	ret = semctl(sem_id, sem_num, GETVAL, 0);
	if(ret == -1)
	{
		printf(" Error on semctl %d(%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Value of semaphore id=%d number=%d value=%d\n", sem_id, sem_num, ret);

	return 0;	
}
