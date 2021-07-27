#define _GNU_SOURCE
/* Code to get the stat of the semaphore */
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <getopt.h>

union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};
#if 0
struct ipc_perm {
               key_t          __key; /* Key supplied to semget(2) */
               uid_t          uid;   /* Effective UID of owner */
               gid_t          gid;   /* Effective GID of owner */
               uid_t          cuid;  /* Effective UID of creator */
               gid_t          cgid;  /* Effective GID of creator */
               unsigned short mode;  /* Permissions */
               unsigned short __seq; /* Sequence number */
 };
#endif

#define ARG_OPTSEMID 0x01
#define ARG_OPTSEMNUM 0x02

#define ARG_OPTALL ( ARG_OPTSEMID | ARG_OPTSEMNUM )

int main(int argc, char* argv[])
{
	int sem_id, sem_num, sem_val, ret, opt, t_value = 0, nr_sem ,index;
	union semun sem1;
	struct semid_ds semIdDs;
	struct passwd *password;
	int args_passed = 0;
	struct  seminfo semInfo;
	bool all = FALSE;

	while ((opt = getopt(argc, argv, "n:i:ht::a")) != -1) 
	{
		switch(opt)
		{
			case 'n':
				sem_num = atoi(optarg);
				args_passed |= ARG_OPTSEMNUM;
				break;
			case 'i':				
				sem_id = atoi(optarg);
				args_passed |= ARG_OPTSEMID;
				break;
			case 'h':
				printf("%20sHELP: %s\n", "", program_invocation_short_name);
				printf("    This utility displays statistics of a semaphore\n");
				printf(" OPTIONS \n -i sem_id : Identifier of the semaphore\n -n sem_num: Semaphore number \n");
				exit(0);
			case 't':
				/* Check if we have a argument */
				if(optarg != 0)
				{
					t_value = atoi(optarg);
					printf("t_value =%d\n", t_value);
				}
				else 
				{
					/* To support mandatory with -t <arg> format */
					if((argv[optind]!= NULL) && (argv[optind][0] != '-'))
					{
						t_value = atoi(argv[optind]);
						optind++;
					}
				}
				break;
			case 'a':
				all = TRUE;
				break;
			case '?':
				printf("USAGE: %s [ -i sem_id ] [ -n sem_num ]\n", program_invocation_short_name);		
				exit(EXIT_FAILURE);
			default:
				printf("USAGE: %s [ -i sem_id ] [ -n sem_num ]\n", program_invocation_short_name);		
				exit(EXIT_FAILURE);
		}
	}

//	printf("t_value=%d\n", t_value);

	if(((args_passed & ARG_OPTALL) != ARG_OPTALL) && (!all))
	{
		if((args_passed & ARG_OPTSEMID) == 0)
		{
			printf(" Semaphore identifier is a required option\n ");
		}
		if((args_passed & ARG_OPTSEMNUM) == 0)
		{
			printf("Semaphore number is a required option\n");
		}

		printf("USAGE: %s [ -i sem_id ] [ -n sem_num ]\n", program_invocation_short_name);		
		exit(EXIT_FAILURE);
	}


	if(all)
	{
		sem1.__buf = &semInfo;
		nr_sem = semctl(0, 0, SEM_INFO, sem1);
		if(nr_sem == -1)
		{
			printf("Error getting value nr_sems%d(%s)", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		for(index = 0; index <= nr_sem; index++)
		{
			ret = semctl(index, 0, SEM_STAT, sem1);
			if(ret == -1)
			{
				printf(" Error on semctl %d(%s)\n", errno, strerror(errno));
				exit(EXIT_FAILURE);
			}

			printf("Semaphore identifier=%d \nSemaphore number=%d \n", sem_id, sem_num);
			printf("Semaphore key=%#x\n", sem1.buf->sem_perm.__key);
			password = getpwuid(sem1.buf->sem_perm.uid);
			if(password != NULL)
			{
				printf("Owner=%s(uid=%ld, gid=%ld)\n", password->pw_name, sem1.buf->sem_perm.uid, sem1.buf->sem_perm.gid);
			}
			password = getpwuid(sem1.buf->sem_perm.cuid);

			if(password != NULL)
			{
				printf("Creater=%s(uid=%ld, gid =%ld)\n", password->pw_name, sem1.buf->sem_perm.cuid, sem1.buf->sem_perm.cgid);
			}

			printf("Mode=%o \nSequence number=%u\n", sem1.buf->sem_perm.mode, sem1.buf->sem_perm.__seq);
			printf("\n\n");
		}

		return 0;
	}
	
	sem1.buf = &semIdDs;
	ret = semctl(sem_id, sem_num, SEM_STAT, sem1);
	if(ret == -1)
	{
		printf(" Error on semctl %d(%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Semaphore identifier=%d \nSemaphore number=%d \n", sem_id, sem_num);
	printf("Semaphore key=%#x\n", sem1.buf->sem_perm.__key);
	password = getpwuid(sem1.buf->sem_perm.uid);
	if(password != NULL)
	{
		printf("Owner=%s(uid=%ld, gid=%ld)\n", password->pw_name, sem1.buf->sem_perm.uid, sem1.buf->sem_perm.gid);
	}
	password = getpwuid(sem1.buf->sem_perm.cuid);

	if(password != NULL)
	{
		printf("Creater=%s(uid=%ld, gid =%ld)\n", password->pw_name, sem1.buf->sem_perm.cuid, sem1.buf->sem_perm.cgid);
	}

	printf("Mode=%o \nSequence number=%u\n", sem1.buf->sem_perm.mode, sem1.buf->sem_perm.__seq);

	return 0;	
}
