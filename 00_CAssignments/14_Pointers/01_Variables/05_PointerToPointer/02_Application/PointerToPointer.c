#include<stdio.h>

int main(void)
{
	//variable declaration
	int num;
	int *ptr = NULL;
	int **pptr = NULL;

	//code
	num = 10;
	printf("\n\n");

	printf("******************Before ptr = &num*****************\n\n");
	printf("Value of 'num'   = %d\n\n", num);
	printf("address of 'num'   = %p\n\n", &num);
	printf(" Value at address of 'num'   = %p\n\n", *(&num));

	ptr = &num;

	printf("\n\n");
	printf("******************Before pptr = &num*****************\n\n");
	printf("Value of 'num'   = %d\n\n", num);
	printf("address of 'num' (ptr)   = %p\n\n", ptr);
	printf("address of 'ptr' (pptr)   = %p\n\n", *pptr);
	printf(" Value at address of 'num' (*ptr)(**ptr)   = %p\n\n", **pptr);
	return(0);
}