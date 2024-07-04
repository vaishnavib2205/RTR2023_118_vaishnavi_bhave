#include <stdio.h>
#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(void)
{
	//variable declarations
	int iArray[NUM_ROWS][NUM_COLUMNS];
	int i, j;

	//code
	///***Name f an Array itself is its base address ***
	//***hence iArray is base address of 2D Integer Array iArray[][]
	//iArray[0] => is the 0th row ... hence is the base address of a row 0


	for (i = 0; i < NUM_ROWS; i++)
	{
		for (j = 0; j < NUM_COLUMNS; j++)
			*(*(iArray + i) + j) = (i + 1) * (j + 1);

	}
	printf("\n\n");
	printf("2D Integer Elements Along With Addresses: \n\n");

	for (i = 0; i < NUM_ROWS; i++)
	{
		for (j = 0; j < NUM_COLUMNS; j++)
		{
			printf(" * (*(iArray + % d) + % d \t \t at address(*iArray + % d) + % d) : % p\n", i, j, *(*iArray + i) + j), i, j, (*(iArray + i) + j);
		}
		printf("\n\n");
	}
	return(0);
}