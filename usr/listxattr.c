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

#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{

	int fd, ret;
	const char *pathName;
	char *buffer = NULL, *pos=0;
	ssize_t bytes, bufflen = 0, bytesLeft;
	char attrBuffer[BUFFER_SIZE] = {'\0'};

	if(argc != 2)
	{
		printf("USAGE: %s </path/to/file>\n", program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	pathName = argv[1];

	bufflen = listxattr(pathName, NULL, 0);
	if (bytes == -1) {
               perror("listxattr");
               exit(EXIT_FAILURE);
        }
	
	if ( bufflen == 0 )
	{
		printf(" No extended attributes available for %s\n", pathName);
		return -1;
	}

	buffer = malloc(bufflen);
	if(buffer == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	bytes = listxattr(pathName, buffer, bufflen);
	if (bytes == -1) {
               perror("listxattr");
               exit(EXIT_FAILURE);
        }

	bytesLeft = bufflen;
	pos = buffer;

	while(bytesLeft)
	{
		bytes = getxattr(pathName,  pos, attrBuffer, BUFFER_SIZE);
		if (bytes == -1)
		{
			printf("%s: ---\n", pos);
		}
		else
		{
			printf("%s:%s \n", pos, attrBuffer);
		}

		bytesLeft -= strlen(pos) + 1;
		pos += strlen(pos) + 1;

		memset(attrBuffer, '\0', BUFFER_SIZE);
	}
	
	close(fd);

	return 0;
}
