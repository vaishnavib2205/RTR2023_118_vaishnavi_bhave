#include <stdio.h>

int main(void) {
	//variable declarations
	int iArray[10];
	int* ptr_iArray = NULL;
	int i;

	//code
	for (i = 0; i < 10; i++)
		iArray[i] = (i + 1) * 3;

	ptr_iArray = iArray;

	printf("\n\n");
	printf("Elements of the Integer Array : \n\n");
	for (i = 0; i < 10; i++)
		printf("iArray[%d] =%d\n", i, *(ptr_iArray + i));

	printf("\n\n");
	printf("Elements of the Integer array: \n\n");
	for (i = 0; i < 10; i++)
		printf("iArray[%d] = %d\n", i, *(ptr_iArray + i));

	printf("\n\n");
	return(0);
}