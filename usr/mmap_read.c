#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>

#define READ_BUFFER_SIZE 4096

int main(int argc, char* argv[])
{
	int 		fd, rc = 0;
	const char 	*pathName = NULL;
	void *buf = NULL;

	if(argc != 2)
	{
		printf(" Invalid arguments passed\n\t\tUSAGE: %s <file_name>\n", argv[0]);
		return -1;
	}

	pathName = argv[1];
	fd = open(pathName, O_RDONLY);
	if(fd == -1)
	{
		printf(" Error %d(%s) opening the path %s\n", errno, strerror(errno), pathName);
		return -2;
	}

	buf = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if(buf == MAP_FAILED)
	{
		rc = errno;
		printf(" Error %d(%s) on mmap\n", rc, strerror(rc));
		goto exit;
	}
	printf("buf=%p content=%s\n", buf, buf);
	getchar();
exit:
	close(fd);
	return rc;
}
