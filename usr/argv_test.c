#include <stdio.h>

int main(int argc, char* argv[])
{
	printf("argv=%p  argc=%p\n", argv, &argc);
	
	getchar();

	return 0;
}
