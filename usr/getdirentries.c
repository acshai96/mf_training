#include <stdio.h> //printf
#include <dirent.h> //getdirectent
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/syscall.h>

#define BUFFER_SIZE 4096

struct linux_dirent64 {
	unsigned long  d_ino;     /* Inode number */
	unsigned long  d_off;     /* Offset to next linux_dirent */
	unsigned short d_reclen;  /* Length of this linux_dirent */
        unsigned char  d_type;   /* File type */
      	char           d_name[];  /* Filename (null-terminated) */
};

int main(int argc, char *argv[])
{
	int fd, ret, i;
	struct linux_dirent64 *ld;
	char buffer[BUFFER_SIZE] = {'\0'};

	if(argc != 2)
	{
		printf("USAGE: %s <path>\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDONLY | O_DIRECTORY);
	if(fd == -1)
	{
		printf("Opening \"%s\" failed with %d(%s)\n", argv[1], errno, strerror(errno));
		return -2;
	}
	while(1)
	{
		ret = syscall(SYS_getdents64, fd, buffer, BUFFER_SIZE);
		if(ret == 0)
		{
			break;
		}
		else if(ret == -1)
		{
			printf("Error occured in executing getdents64\n");
			return -2;
		}
		for (i = 0; i < ret;)
		{
			ld = (struct linux_dirent64 *)&buffer[i];
			printf("\e[34m");
			printf("%s ", ld->d_name);
			i += ld->d_reclen;
		}	
	}

	printf("\n");

	close(fd);

	return 0;
}
