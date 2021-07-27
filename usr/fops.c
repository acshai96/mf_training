#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int fd;
	char readBuffer[512] = {'1'};
	size_t ret;

	if(argc != 2)
	{
		printf("USAGE :%s <file_name>\n", argv[0]);
		return -1;
	}
	fd = open(argv[1], O_RDWR | O_DIRECTORY);
	if(fd == -1)
	{
		printf(" Error %d(%s) opening the path %s\n", errno, strerror(errno), argv[1]);
		return -2;
	}
	getchar();
	ret = read(fd, (void *)readBuffer, 10);
        if(ret != 10)
        {
            printf(" Error %d(%s) in read operation\n", errno, strerror(errno));
//            return -3;
        }
	else
	{
		printf("readData=%s\n", readBuffer);
	}

	ret = write(fd, (void*)readBuffer, 10);
	if(ret != 10)
	{	
            printf(" Error %d(%s) in write operation\n", errno, strerror(errno));
	}
	close(fd);

	return 0;
}
