#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
	pid_t pid = 0, pid1 = 0;
	int waitstatus = 0;

	pid = fork();
	if(pid == (pid_t)-1)
	{
		printf("Fork called failed\n");
		return -1;
	}
	/* Child process*/
	if(pid == 0)
	{
		char ch= '\0';

		printf("In Childprocess pid %d ppid %d", getpid(), getppid());
		ch = getchar();
		if(ch == 'e')
		{
			return 24;
		}
		exit(0);
	}

	printf("In parent process pid%d\n", getpid());
	pid1 = wait(&waitstatus);
	if(pid1 != pid)
	{
		printf("Wait call failed with ret=%d\n", pid1);
		return -1;
	}
	
	if(WIFEXITED(waitstatus))
	{
		if(WEXITSTATUS(waitstatus))
		{
			printf(" Child exited with err=%d\n", WEXITSTATUS(waitstatus));
		}
		else
		{
			printf("Child exited normally\n");
		}
	}
	return 0;
}
