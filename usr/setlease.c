#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

void sigHandler(int sig)
{
	printf("In %s caught signal %d\n", __func__, sig);

}

int main(int argc, char *argv[])
{

	int fd, ret;
	const char *pathName;

	if(argc != 2)
	{
		printf("USAGE: %s </path/to/file>\n", program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	pathName = argv[1];

	errno = 0;
	fd = open(pathName, O_WRONLY);
	if(fd == -1)
	{
		printf("Error %d(%s)opening \'%s\' \n", errno, strerror(errno), pathName);
		exit(EXIT_FAILURE);
	}
	
	signal(SIGIO, sigHandler);
	errno = 0;	
	ret = fcntl(fd, F_SETLEASE, F_WRLCK);
	if (ret == -1)
	{
		printf("Error %d(%s) in fcntl \n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	while(1)
	{
		pause();
	}

	close(fd);

	return 0;
}
