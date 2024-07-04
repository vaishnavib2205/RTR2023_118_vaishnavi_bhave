#include <stdio.h>

int main(void)
{
	//variable declarations
	double dArray[10];
	int i;

	//code
	for (i = 0; i < 10; i++)
		dArray[i] = (double)(i + 1) * 1.5f;
	printf("\n\n");
	printf("Elements of the 'double' array : \n\n");
	for (i = 0; i < 10; i++)
		printf("dArray[%d] = %f\n", i, dArray[i]);

	printf("\n\n");
	printf("Elements of the 'double' array: \n\n");
	for (i = 0; i < 10; i++)
		printf("dArray[%d] = %lf  \t \t Address = %p\n", i, dArray[i], &dArray[i]);

	printf("\n\n");
	return(0);
}