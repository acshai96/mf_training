#define _GNU_SOURCE

/* Program to experiment subopt */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#define VOLFLAG_RO	0x01
#define VOLFLAG_RW	0x02

int main(int argc, char*argv[])
{
	char *volName = NULL, *mntPoint = NULL, *poolName = NULL;
	int opt, flags;
	char *sopts, ret, *value, *endPtr;
	long volId;

	enum {
		OPT_VOLNAME = 0,
		OPT_MNTPNT,
		OPT_POOLNAME,
		OPT_RO,
		OPT_RW,
		OPT_VOLID
	};

	char *const token[] = {
		[OPT_VOLNAME] 	= "volName",
		[OPT_MNTPNT] 	= "mntPoint",
		[OPT_POOLNAME] 	= "poolName",
		[OPT_RO] 	= "readOnly",
		[OPT_RW]	= "readWrite",
		[OPT_VOLID]	= "volId",
		NULL
	};

	while((opt = getopt(argc, argv, "o:h")) != -1)
	{
		switch(opt)
		{
			case 'o':
				sopts = optarg;
				while((ret = getsubopt(&sopts, token, &value)) != -1)
				{
					printf("sopts=%s value=%s\n", sopts, value);
					switch(ret)
					{
						case OPT_VOLNAME:
							if (value == NULL)
							{
								printf("Missing value for volume name \n");
								exit(EXIT_FAILURE);	
							}
							volName = value;
							break;
						case OPT_MNTPNT:
							if (value == NULL)
							{
								printf("Missing value for mount path \n");
								exit(EXIT_FAILURE);	
							}
							mntPoint = value;
							break;
						case OPT_POOLNAME:
							if (value == NULL)
							{
								printf("Missing value for pool name \n");
								exit(EXIT_FAILURE);	
							}
							poolName = value;
							break;
						case OPT_RO:
							flags |= VOLFLAG_RO;
							break;
						case OPT_RW:
							flags |= VOLFLAG_RW;
							break;
						case OPT_VOLID:
							volId = strtol(value, &endPtr, 10);
							 if ((errno == ERANGE && (volId == LONG_MAX || volId == LONG_MIN))
										|| 
								(errno != 0 && volId == 0))
							{
								printf("volume Id value out of range\n");
								exit(EXIT_FAILURE);
							}
							if(endPtr == value)
							{
								printf(" volumeId not in integer format\n");
								exit(EXIT_FAILURE);
							}
							break;
						default:
							printf("Unrecognized option %d \n ", ret);
							exit(EXIT_FAILURE);
					}
				}
				break;
			case 'h':
				printf("Help");
				exit(0);
			case '?':
			default:
				printf("USAGE %s [ -o options]\n", program_invocation_short_name);
				exit(EXIT_FAILURE);
		}
	}

	printf(" Outside while sopts=%s value=%s\n", sopts, value);
	if((flags & VOLFLAG_RO) && (flags & VOLFLAG_RW))
	{
		printf("Volume cannot in be both readonly and readwrite \n");
		exit(EXIT_FAILURE);
	}

	printf(" poolName=%s mntPoint=%s volName=%s flags=%s%s volId=%ld\n", 
					poolName, 
					mntPoint,
					volName,
					(flags & VOLFLAG_RO) ? "readOnly": "",
					(flags & VOLFLAG_RW) ? "readWrite":"",
					volId);

	return 0;
}
