#include<stdio.h>

// DEFINING STRUCT 

struct MyData
{
	int i;
	float f;
	double d;

};

int main(void)
{
	// variable declaration
	int i_size;
	int f_size;
	int d_size;
	int struct_MyData_size;
	int pointer_to_struct_MyData_Size;

	struct MyData *pData = NULL;

	// code
	printf("\n\n");

	pData = (struct MyData *)malloc(sizeof(struct MyData));

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
	(*pData).i = 30;
	(*pData).f = 11.45f;
	(*pData).d = 1.2995;

	// Displaying Values of the data members 

	printf("\n\n");
	printf("Data Members Of struct MyData Are : \n\n");
	printf("i = %d\n", (*pData).i);
	printf("f = %f\n", (*pData).f);
	printf("d = %lf\n", (*pData).d);

	i_size = sizeof((*pData).i);
	f_size = sizeof((*pData).f);
	d_size = sizeof((*pData).d);

	// Displaying sizes in bytes of the data membewrs

	printf("\n\n");
	printf("SIZES OF DATA MEMBERS OF struct MyData ARE : \n\n");
	printf("size of i = %d bytes\n", i_size);
	printf("size of f = %d bytes\n", f_size);
	printf("size of d = %d bytes\n", d_size);

	// Calculating Size Of the entire struct MyDATA

	struct_MyData_size = sizeof(struct MyData);
	pointer_to_struct_MyData_Size = sizeof(struct MyData *);
	
	// Displaying Sizes Of Entire struct MyData
	printf("\n\n");
	printf("Size of 'struct Mydata' : %d \n\n", struct_MyData_size);
	printf("Size of pointer to struct MyData : %d Bytes\n\n", pointer_to_struct_MyData_Size);

	if (pData)
	{
		free(pData);
		pData = NULL;
		printf("Memory Allocated To struct Myata Has Been Successfully Freed !!!\n\n");

	}
	return(0);
}