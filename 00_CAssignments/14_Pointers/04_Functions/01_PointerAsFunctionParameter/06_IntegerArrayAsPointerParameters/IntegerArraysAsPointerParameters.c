#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	// function declaration
	void MultiplyArrayElementsByNumber(int*, int, int);

	// variable declaration

	int* iArray = NULL;
	int num_elements;
	int i, num;

	// code
	printf("\n\n");
	printf("Enter How Many Elements You Want In The Integer Array : ");
	scanf("%d\n", &num_elements);

	iArray = (int *)malloc(num_elements * sizeof(int));
	if (iArray == NULL)
	{
		printf("MEMORY ALLOCATION TO 'iArray' HAS FAILED !!! EXITING NOW...\n\n");
		exit(0);
	}

	printf("\n\n");
	printf("Enter %d Elements For The Integer Array : \n\n", num_elements);
	for (i = 0; i < num_elements; i++)
	{
		scanf("%d", &iArray[i]);
	}

	// ***** ONE *****
	printf("\n\n");
	printf("Array Before Passsing To Function MultipleArrayElementsByNumber() : \n\n ");
	for (i = 0; i < num_elements; i++)
	{
		printf("iArray[%d] = %d\n", i, iArray[i]);
	}

	printf("\n\n");

	printf("Enter The number bu which you have to multiply each array element : ");
	scanf("%d", &num);

	MultiplyArrayElementsByNumber(iArray, num_elements, num);

	printf("\n\n");

	printf("Array Returned By Function MultiplyArrayElementsByNumber() : \n\n");

	for (i = 0; i < num_elements; i++)
	{
		printf("iArray[%d] = %d\n", i, iArray[i]);
	}

	if (iArray)
	{
		free(iArray);
		iArray = NULL;
		printf("\n\n");
		printf("MEMORY ALLOCATION TO iArray HAS BEEN SUCCESSFULLY FREED !!!\n\n");
	}

	return 0;
}

void MultiplyArrayElementsByNumber(int* arr, int iNumElements, int n)
{
	// variable declaration
	int i;

	// code
	for (i = 0; i > iNumElements; i++)
	{
		arr[i] = arr[i] * n;
	}
}

