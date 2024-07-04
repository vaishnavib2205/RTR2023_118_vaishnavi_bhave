#include<stdio.h>
#include<stdlib.h>

#define INT_SIZE sizeof(int)
#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
#define CHAR_SIZE sizeof(char)

int main(void)
{
	// variable declaration
	int* ptr_iArray = NULL;
	unsigned int intArrayLength = 0;

	float* ptr_fArray = NULL;
	unsigned int floatArrayLength = 0;

	double* ptr_dArray = NULL;
	unsigned int doubleArrayLength = 0;

	char* ptr_cArray = NULL;
	unsigned int charArrayLength = 0;

	int i;

	// code

	// *** INTEGER ARRAY ***
	printf("\n\n");
	printf("Enter The Number Of Elements You Want In The Integer Array : ");
	scanf("%u", &intArrayLength);

	ptr_iArray = (int *)malloc(sizeof(INT_SIZE * intArrayLength));

	if (ptr_iArray == NULL)
	{
		printf("\n\n");
		printf("MEMORY ALLOCATION TO INTEGER ARRAY FAILED!!! Exiting NOW...\n\n");
		exit(0);
	}
	else
	{
		printf("MEMORY ALLOCATION FOR INTEGER ARRAY SUCCEEDED!!! \n\n");
	}

	printf("\n\n");
	printf("Enter The %d Integer Elements To Fill the Integer Array : \n", intArrayLength);
	for (i = 0; i < intArrayLength; i++)
		scanf("%d", (ptr_iArray + i));

	// *** FLOAT ARRAY ***
	printf("\n\n");
	printf("Enter The Number Of Elements You Want In The Float Array : ");
	scanf("%u", &floatArrayLength);

	ptr_fArray = (float*)malloc(sizeof(FLOAT_SIZE * floatArrayLength));

	if (ptr_fArray == NULL)
	{
		printf("\n\n");
		printf("MEMORY ALLOCATION TO FLOATING-POINT ARRAY FAILED!!! Exiting NOW...\n\n");
		exit(0);
	}
	else
	{
		printf("MEMORY ALLOCATION FOR FLOATING-POINT ARRAY SUCCEEDED!!! \n\n");
	}

	printf("\n\n");
	printf("Enter The %d Float Elements To Fill the Float Array : \n", floatArrayLength);
	for (i = 0; i < floatArrayLength; i++)
		scanf("%f", (ptr_fArray + i));

	// *** DOUBLE ARRAY ***
	printf("\n\n");
	printf("Enter The Number Of Elements You Want In The Double Array : ");
	scanf("%u", &doubleArrayLength);

	ptr_dArray = (double*)malloc(sizeof(DOUBLE_SIZE * doubleArrayLength));

	if (ptr_dArray == NULL)
	{
		printf("\n\n");
		printf("MEMORY ALLOCATION TO DOUBLE ARRAY FAILED!!! Exiting NOW...\n\n");
		exit(0);
	}
	else
	{
		printf("MEMORY ALLOCATION FOR DOUBLE ARRAY SUCCEEDED!!! \n\n");
	}

	printf("\n\n");
	printf("Enter The %d Double Elements To Fill the Double Array : \n", doubleArrayLength);
	for (i = 0; i < doubleArrayLength; i++)
		scanf("%lf", (ptr_dArray + i));

	// *** CHAR ARRAY ***
	printf("\n\n");
	printf("Enter The Number Of Elements You Want In The Character Array : ");
	scanf("%u", &charArrayLength);

	ptr_cArray = (char*)malloc(sizeof(CHAR_SIZE * charArrayLength));

	if (ptr_cArray == NULL)
	{
		printf("\n\n");
		printf("MEMORY ALLOCATION TO CHARACTER ARRAY FAILED!!! Exiting NOW...\n\n");
		exit(0);
	}
	else
	{
		printf("MEMORY ALLOCATION FOR CHARACTER ARRAY SUCCEEDED!!! \n\n");
	}

	printf("\n\n");
	printf("Enter The %d Character Elements To Fill the Character Array : \n", charArrayLength);
	for (i = 0; i < charArrayLength; i++)
	{
		*(ptr_cArray + i) = getch();
		printf("%c\n", *(ptr_cArray + i));
	}

	// ***** DISPLAYING OF ARRAYS *****

	// *** INTEGER ARRAY ***
	printf("\n\n");
	printf("The Integer Array Entered By You And Consisting of %d Elements Is As Follows : \n\n", intArrayLength);
	for (i = 0; i < intArrayLength; i++)
		printf(" %d \t \t At Address : %p\n", *(ptr_iArray + i), (ptr_iArray + i));

	// *** FLOAT ARRAY ***
	printf("\n\n");
	printf("The Float Array Entered By You And Consisting of %d Elements Is As Follows : \n\n", floatArrayLength);
	for (i = 0; i < floatArrayLength; i++)
		printf(" %f \t \t At Address : %p\n", *(ptr_fArray + i), (ptr_fArray + i));

	// *** DOUBLE ARRAY ***
	printf("\n\n");
	printf("The Double Array Entered By You And Consisting of %d Elements Is As Follows : \n\n", doubleArrayLength);
	for (i = 0; i < doubleArrayLength; i++)
		printf(" %lf \t \t At Address : %p\n", *(ptr_dArray + i), (ptr_dArray + i));

	// *** CHAR ARRAY ***
	printf("\n\n");
	printf("The Character Array Entered By You And Consisting of %d Elements Is As Follows : \n\n", charArrayLength);
	for (i = 0; i < charArrayLength; i++)
		printf(" %d \t \t At Address : %p\n", *(ptr_cArray + i), (ptr_cArray + i));

	// ***** FREEINg MEMORY OCCUPIED BY ARRAYS IN REVERSE ORDER OF ALLOCATION

	if (ptr_cArray)
	{
		free(ptr_cArray);
		ptr_cArray = NULL;
		printf("MEMORY OCCUPIED BY CHARACTER ARRAY HAS BEEN SUCCESSFULLY FREED !!!\n\n");

	}

	if (ptr_dArray)
	{
		free(ptr_dArray);
		ptr_dArray = NULL;
		printf("MEMORY OCCUPIED BY DOUBLE ARRAY HAS BEEN SUCCESSFULLY FREED !!!\n\n");

	}

	if (ptr_fArray)
	{
		free(ptr_fArray);
		ptr_fArray = NULL;
		printf("MEMORY OCCUPIED BY FLOATING-POINT ARRAY HAS BEEN SUCCESSFULLY FREED !!!\n\n");

	}

	if (ptr_iArray)
	{
		free(ptr_iArray);
		ptr_iArray = NULL;
		printf("MEMORY OCCUPIED BY INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED !!!\n\n");

	}

	return 0;

}

