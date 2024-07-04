#include <stdio.h>
#include <stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(void)
{
	//variable declarations
	int* iArray[NUM_ROWS];//a 2D array which will have 5 rows and number of columns can be decided later on...
	int i, j;
	//code
	printf("\n\n");
	for (i = 0; i < NUM_ROWS; i++)
	{
		iArray[i] = (int*)malloc(NUM_COLUMNS * sizeof(int));
		if (iArray[i] = NULL)
		{
			printf("FAILED TO ALLOCATE MEMORY TO ROW %D OF 2D INTEGER ARRAY!! EXITTING NOW...\n\n", i);
			exit(0);
		}
		else
			printf("MEMORY ALLOCATION TO ROW %d OF 2D INTEGER ARRAY SUCCEEDED!!!\n\n", i);

	}


	//Assigning values to 2D array...
	for (i = 0; i < NUM_ROWS; i++)
	{
		for (j = 0; j < NUM_COLUMNS; j++)
		{
			iArray[i][j] = (i + 1) * (j + 1);
		}
	}

	//displaying 2D array
	printf("\n\n");
	printf("Displaying 2D Array: \n\n");
	for (i = 0; i < NUM_ROWS; i++) {
		for (j = 0; j < NUM_COLUMNS; j++) {
			printf("iArray[%d][%d] = %d\n", i, j, iArray[i][j]);
		}
		printf("\n\n");
	}
	printf("\n\n");

	//Displaying Memory Assigned to 2D array (must be done in reverse order
	for (i = (NUM_ROWS - 1); i >= 0; i--)
	{
		free(iArray[i]);
		iArray[i] = NULL;

		printf("MEMORY ALLOCATED TO ROW %d of 2D INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!!!\n\n", i);

	}
	return(0);
}