#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_handler(void *num)
{
	sleep(1);
	printf("%s  num=%d \n", __func__,*(int *)num);
}

int main()
{
	int ret, index;
	pthread_t pth[5];

	for ( index = 0; index < 5; index++)
	{
		ret = pthread_create(&pth[index], NULL, &thread_handler,(void *) &index);
		pthread_join(pth[index], NULL);
	}

	printf("after create");

	return 0;
}
