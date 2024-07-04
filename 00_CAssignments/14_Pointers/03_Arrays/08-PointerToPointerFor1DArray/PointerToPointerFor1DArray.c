#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	void MyAlloc(int** ptr, unsigned int numberOfElements);

	//varaible declarations

	int* piArray = NULL;
	unsigned int num_elements;
	int i;

	//code

	printf("\n\n");
	printf("How Many Elements You Want In Integer Array ?\n\n");
	scanf("%u", &num_elements);

	printf("\n\n");
	MyAlloc(&piArray, num_elements);

	printf("Enter %u Elements To Fill Up Your Integer Array : \n\n", num_elements);
	for (i = 0; i < num_elements; i++)
	{
		scanf("%d", &piArray[i]);
	}

	printf("\n\n");
	printf("Enter %u Elements Entered You in Integer Array : \n\n", num_elements);
	for (i = 0; i < num_elements; i++)
	{
		printf("%u\n", piArray[i]);
	}

	printf("\n\n");

	if (piArray)
	{
		free(piArray);
		piArray = NULL;
		printf("MEMORY ALOCATED HAS BEEN FREED !!!\n\n");
	}

	return 0;
}

void MyAlloc(int** ptr, unsigned int numberOfElements)
{
	// code
	*ptr = (int*)malloc(numberOfElements * sizeof(int));
	if (*ptr == NULL)
	{
		printf("Could Not Allocate Memory !!! Exiting Now...\n\n");
		exit(0);
	}
	printf("MyAlloc() Has Successfully Allocated %zu Bytes For Integer Array !!!\n\n", (numberOfElements * sizeof(int)));
}