#define _GNU_SOURCE

#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	int sem_id, sem_num, ret;
	int val = 0;
	struct sembuf semOp;

	if(argc != 3)
	{
		printf("USAGE: %s <sem_id> <sem_num>\n", program_invocation_short_name);
		exit(EXIT_FAILURE);
	}
	
	sem_id = atoi(argv[1]);
	sem_num = atoi(argv[2]);
	
	semOp.sem_num = sem_num;        /* Operate on semaphore sem_num */
        semOp.sem_op  = -1;         	/* Wait for value to equal to or greater than 1 */
        semOp.sem_flg = 0;

	printf("Requesting for semaphore %d\n", sem_id);
	
	ret = semop(sem_id, &semOp, 1);
	if( ret == -1)
	{
		printf("Error ocurred on semop %d(%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Semaphore %d access granted\n", sem_id);

	return 0;	
}
