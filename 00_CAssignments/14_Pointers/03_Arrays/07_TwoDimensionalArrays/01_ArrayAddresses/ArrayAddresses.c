#include <stdio.h>
#include <stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(void)
{
	//variable declarations
	int iArray[NUM_ROWS][NUM_COLUMNS];
	int i, j;

	//code
	for (i = 0; i < NUM_COLUMNS; i++)
	{
		for(j = 0; j < NUM_ROWS; j++)
			iArray[i][j] = (i + 1) * (j + 1);

    }
	printf("\n\n");
	printf("2D Integer Array ELements Along With Addresses :  \n\n");
	for (i = 0; i < NUM_ROWS; i++)
	{
		for (j = 0; j < NUM_COLUMNS; j++)
		{
			printf("iArray[%d][%d] = %d \t \t At Address : %p\n", i, j, iArray[i][j], &iArray[i][j]);
		}
		printf("\n\n");
	}
	return(0);
}