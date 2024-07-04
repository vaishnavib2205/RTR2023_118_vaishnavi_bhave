#include <stdio.h>
#include <stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS_ONE 3
#define NUM_COLUMNS_TWO 8

int main(void) {
	//variable declarations
	int* iArray[NUM_ROWS];

	int i, j;

	printf("\n\n");
	printf("*********First Memory Allocation To 2D Integer Array**********\n\n");

	for (i = 0; i < NUM_ROWS; i++)
	{
		iArray[i] = (int*)malloc(NUM_COLUMNS_ONE * sizeof(int));
		if (iArray[i] == NULL)
		{
			printf("FAILED TO ALLOCATE MEMORY TO ROW %D OF 2D INTEGER ARRAY!!! EXITTING NOW...\n\n", i);
			exit(0);
		}
		else
			printf("MEMORY ALLOCATION TO ROW %d OF 2D INTEGER ARRAY SUCCEEDED!!!\n\n", i);
		exit(0);
	}

	//assigning values to 2d array
	for (i = 0; i < NUM_ROWS; i++)
	{
		for (j = 0; j < NUM_COLUMNS_ONE; j++)
		{
			iArray[i][j] = (i + 1) * (j + 1);
		}
	}
	//Displaying 2D Array
	printf("\n\n");
	printf("Displaying 2D Array : \n\n");
	for (i = 0; i < NUM_ROWS; i++)
	{
		for (j = 0; j < NUM_COLUMNS_ONE; j++)
		{
			printf("iArray[%d][%d] = %d\n", i, j, iArray[i][j]);
		}
		printf("\n\n");

	}
	printf("\n\n");

	//FREEING MEMORY TO 2D ARRAY MUST BE DONE IN REVERSE ORDER 
	for (i = (NUM_ROWS - 1); i >= 0; i--)
	{
		free(iArray[i]);
		iArray[i] = NULL;
		printf("MEMORY ALLOCATED TO ROW %d OF INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!!!\n\n",i);

	}


	printf("\n\n");
	printf("*********Second Memory Allocation To 2D Integer Array**********\n\n");

	for (i = 0; i < NUM_ROWS; i++)
	{
		iArray[i] = (int*)malloc(NUM_COLUMNS_TWO * sizeof(int));
		if (iArray[i] == NULL)
		{
			printf("FAILED TO ALLOCATE MEMORY TO ROW %D OF 2D INTEGER ARRAY!!! EXITTING NOW...\n\n", i);
			exit(0);
		}
		else
			printf("MEMORY ALLOCATION TO ROW %d OF 2D INTEGER ARRAY SUCCEEDED!!!\n\n", i);
		exit(0);
	}

	//assigning values to 2d array
	for (i = 0; i < NUM_ROWS; i++)
	{
		for (j = 0; j < NUM_COLUMNS_TWO; j++)
		{
			iArray[i][j] = (i + 1) * (j + 1);
		}
	}
	//Displaying 2D Array
	printf("\n\n");
	printf("Displaying 2D Array : \n\n");
	for (i = 0; i < NUM_ROWS; i++)
	{
		for (j = 0; j < NUM_COLUMNS_TWO; j++)
		{
			printf("iArray[%d][%d] = %d\n", i, j, iArray[i][j]);
		}
		printf("\n\n");

	}
	printf("\n\n");

	//FREEING MEMORY TO 2D ARRAY MUST BE DONE IN REVERSE ORDER 
	for (i = (NUM_ROWS - 1); i >= 0; i--)
	{
		free(iArray[i]);
		iArray[i] = NULL;
		printf("MEMORY ALLOCATED TO ROW %d OF INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!!!\n\n",i);

	}



	return 0;
}