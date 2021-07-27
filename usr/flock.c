#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int fd, ret;
	char *pathName = NULL, ch;
	struct flock fl;

	if(argc != 2)
	{
		printf("USAGE: %s <pathName>\n", argv[1]);
		return -1;
	}

	pathName = argv[1];
	fd = open(pathName, O_RDWR);
	if(fd == -1)
	{
		printf("Error %d(%s)opening file %s\n", errno, strerror(errno), pathName);
		return -2;
	}
	
	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_len = 100;
	fl.l_whence = SEEK_SET;

	ret = fcntl(fd, F_SETLKW, &fl);
	if(ret == -1)
	{
		printf("Error %d(%s)acquiring file lock \n", errno, strerror(errno));
		return -2;
	}

	ch = getchar();
	return 0;
}
