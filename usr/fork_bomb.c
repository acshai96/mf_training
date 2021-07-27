#include <stdio.h>
#include <unistd.h>

int main()
{
	pid_t ret = 0;
	while(1)
	{
		ret = fork();
		if(ret == 0)
		{
			printf("In Childprocess pid %d ppid %d\n", getpid(), getppid());
			continue;
		}
		else
		{
			printf("parent process pid%d\n", getpid());
			continue;
		}
	}
	return 0;
}
