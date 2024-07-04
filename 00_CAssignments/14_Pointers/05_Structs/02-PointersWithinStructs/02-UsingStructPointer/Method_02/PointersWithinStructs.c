#include<stdio.h>
#include<stdlib.h>

//Defining Struct

struct MyData
{
	int* ptr_i;
	int i;

	float* ptr_f;
	float f;

	double* ptr_d;
	double d;
};

int main(void)
{
	struct MyData* pData = NULL;

	// code
	printf("\n\n");

	pData = (struct MyData*)malloc(sizeof(struct MyData));

	if (pData == NULL)
	{
		printf("FAILED TO ALLOCATE MEMORY TO 'struct MyData' !!! Exiting NOW...\n\n");
		exit(0);
	}
	else
	{
		printf("SUCCESSFULLY ALLOCATED MEMORY TO 'struct MyData' !!! \n\n");
	}

	pData->i = 9;
	pData->ptr_i = &pData->i;

	pData->f = 11.45f;
	pData->ptr_f = &pData->f;

	pData->d = 30.121995;
	pData->ptr_d = &pData->d;

	printf("\n\n");
	printf("i = %d\n", *(pData->ptr_i));
	printf("Address of i = %p\n", pData->ptr_i);

	printf("\n\n");
	printf("f = %f\n", *(pData->ptr_f));
	printf("Address of f = %p\n", pData->ptr_f);

	printf("\n\n");
	printf("d = %lf\n", *(pData->ptr_d));
	printf("Address of d = %p\n", pData->ptr_d);

	if (pData)
	{
		free(pData);
		pData = NULL;
		printf("Memory Allocated To struct Myata Has Been Successfully Freed !!!\n\n");
	}

	return 0;

}
