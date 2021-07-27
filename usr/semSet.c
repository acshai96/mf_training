#define _GNU_SOURCE

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

	if(argc != 4)
	{
		printf("USAGE: %s <sem_id> <sem_num> <sem_val>\n", program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	sem_id = atoi(argv[1]);
	sem_num = atoi(argv[2]);
	sem_val = atoi(argv[3]);
	sem1.val = sem_val;

	ret = semctl(sem_id, sem_num, SETVAL, sem1);
	if(ret == -1)
	{
		printf(" Error on semctl %d(%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	return 0;	
}
