#include <stdio.h>
int main(void)
{
	//function declarations
	void SwapNumbers(int*, int*);

	//variable declarations
	int a;
	int b;

	//code
	printf("\n\n");
	printf("Enter value for 'A': ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter value for 'B' : ");
	scanf("%d", &b);

	printf("\n\n");
	printf("***********Before Swapping*********\n\n");
	printf("Value of 'A': %d\n\n", a);
	printf("Value of 'B': %d\n\n", b);

	SwapNumbers(&a, &b);

	printf("\n\n");
	printf("***********After Swapping************\n\n");
	printf("Value of 'A' = %d\n\n", a);
	printf("Value of 'B' = %d\n\n", b);

	return(0);
}

void SwapNumbers(int* x, int* y)
{
	int temp;
	printf("\n\n");
	printf("*******************BEFORE SWAPPING**************\n\n");
	printf("Value of 'X' = %d\n\n", *x);
	printf("Value of 'Y' = %d\n\n", *y);

	temp = *x;
	*x = *y;
	*y = temp;

	printf("\n\n");
	printf("*******************AFTER SWAPPING**************\n\n");
	printf("Value of 'X' = %d\n\n", *x);
	printf("Value of 'Y' = %d\n\n", *y);
}