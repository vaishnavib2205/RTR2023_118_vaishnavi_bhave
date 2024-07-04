#include <stdio.h>
int main(void)
{
	//variable declarations
	int VB_i;

	//code
	printf("\n\n");

	printf("Printing digits 10 to 1: \n\n");

	VB_i = 10;

	do {
		printf("\t%d\n", VB_i);
		VB_i--;
	} while (VB_i >= 1);

	printf("\n\n");

	return(0);
}