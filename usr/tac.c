#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define READ_BUFFER_SIZE 4096

int main(int argc, char* argv[])
{
	int 		fd, rc = 0;
	const char 	*pathName = NULL;
	off_t 		fileSize, bytesLeft, readSize;
	char 		readBuffer[READ_BUFFER_SIZE] = {'\0'}, lastBuffer[READ_BUFFER_SIZE] = {'\0'};
	char        *pos, *pos1;
	size_t 		ret;
	bool        leftOver = false;

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

	/* Get the length of the file(in bytes) */
	fileSize = lseek(fd, 0, SEEK_END);
	if(fileSize == (off_t) -1)
	{
		printf(" Error %d(%s) in seek operation\n", errno, strerror(errno));
		rc = -3;
		goto exit;
	}

    bytesLeft = fileSize;
    while(bytesLeft != 0)
    {
        readSize = (bytesLeft < READ_BUFFER_SIZE) ? bytesLeft : READ_BUFFER_SIZE;

        /* read the contents from the end window */
        ret = pread(fd, (void *)readBuffer, readSize, (bytesLeft - readSize));
        if(ret != readSize)
        {
            printf(" Error %d(%s) in read operation\n", errno, strerror(errno));
            rc = -4;
            goto exit;
        }

        bytesLeft -= readSize;
        pos1 = readBuffer;
        /* Display line by line */
        while(1)
        {
            pos = strrchr(readBuffer, '\n');
            if(pos == NULL)
            {
                if(strlen(readBuffer) > 0)
                {
                    if(bytesLeft == 0)
                    {
                        printf("%s\n", readBuffer);
                    }
                    else
                    {
                        strcpy(lastBuffer, readBuffer);
                        leftOver = true;
                    }
                }
                memset(readBuffer, '\0', READ_BUFFER_SIZE);
                break;
            }
            printf("%s%s\n", pos + 1, (leftOver)?(lastBuffer):(""));

            if(leftOver)
            {
                    memset(lastBuffer, '\0', READ_BUFFER_SIZE);
                    leftOver = false;
            }
            *pos = '\0';
        }
    }

exit:
	close(fd);
	return rc;
}
