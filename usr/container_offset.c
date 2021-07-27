/* Implementation of offsetof */

#include <stdio.h>   //printf
#include <stdint.h>  //integer types
#include <stdbool.h> //bool

#define offsetof(structName, structMember) ((int)((void *)&((struct structName *)0)->structMember - (void *)0))

#define containerof(structName, structMember, membAddr) (struct StructName *)(((void *)membAddr) - offsetof(structName, structMember))

struct emp {
	int empId;
	char name[35];
	uint64_t salary;
	bool isManager;
};

int main()
{
	struct emp myemp1;

	printf("offsetof(empID)=%d\noffsetof(name)=%d\noffsetof(salary)=%d\noffsetof(isManager)=%d\n",
			offsetof(emp, empId), offsetof(emp, name), offsetof(emp, salary), offsetof(emp, isManager));

	printf("myemp1=%p containerof()=%p\n", &myemp1, containerof(emp, salary, &myemp1.salary));

	return 0;
}
