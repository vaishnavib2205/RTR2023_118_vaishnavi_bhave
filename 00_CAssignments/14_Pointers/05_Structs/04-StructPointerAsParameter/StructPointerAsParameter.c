#include<stdio.h>
#include<stdlib.h>

// DEFINING STRUCT 

struct MyData
{
	int i;
	float f;
	double d;

};

int main(void)
{
	// function prototype

	void ChangeValues(struct MyData*);

	// variable declaration

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

	// Assigning Datas Values to yhe data mamber of dtruct MyData 
	pData->i = 30;
	pData->f = 11.45f;
	pData->d = 1.2995;

	// Displaying Values of the data members 

	printf("\n\n");
	printf("Data Members Of struct MyData Are : \n\n");
	printf("i = %d\n", pData->i);
	printf("f = %f\n", pData->f);
	printf("d = %lf\n", pData->d);

	ChangeValues(pData);

	// Displaying values

	printf("\n\n");
	printf("DATA MEMBERS Of 'struct MyData' ARE : \n\n");
	printf("i = %d\n", pData->i);
	printf("f = %f\n", pData->f);
	printf("d = %lf\n", pData->d);


	if (pData)
	{
		free(pData);
		pData = NULL;
		printf("Memory Allocated To struct Myata Has Been Successfully Freed !!!\n\n");

	}
	return(0);
}

void ChangeValues(struct MyData* pParam_Data)
{
	// code

	pParam_Data->i = 9;
	pParam_Data->f = 8.2f;
	pParam_Data->d = 6.1998;

	// can also do this....

	// (*pParam_data).i = 9;

}