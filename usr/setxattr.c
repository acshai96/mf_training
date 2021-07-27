#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{

	int fd, ret;
	const char *pathName;
	const char *xattrName;
	const char *xattrValue;
	ssize_t bytes, bufflen = 0;

	if(argc != 4)
	{
		printf("USAGE: %s </path/to/file> <xattr_name> <xattr_value> \n", program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	pathName = argv[1];
	xattrName = argv[2];
	xattrValue = argv[3];
	

	bytes = setxattr(pathName, xattrName, xattrValue, strlen(xattrValue), XATTR_CREATE);
	if (bytes == -1) {
               perror("setxattr");
               exit(EXIT_FAILURE);
        }
	printf("xattr %s:%s added for %s\n", xattrName, xattrValue, pathName);
	close(fd);

	return 0;
}
