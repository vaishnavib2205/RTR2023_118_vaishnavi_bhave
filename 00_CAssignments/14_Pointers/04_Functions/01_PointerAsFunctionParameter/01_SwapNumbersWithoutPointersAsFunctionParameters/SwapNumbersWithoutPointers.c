#include <stdio.h>

int main(void)
{
	int a;
	int b;
	void SwapNumbers(int, int);
	//code
	printf("\n\n");
	printf("Enter value for 'A' : ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Value for 'B' : ");
	scanf("%d", &b);

	printf("\n\n");
	printf("********BEFORE SWAPPING*********\n\n");
	printf("Value of 'A' = %d\n\n", a);

	SwapNumbers(a, b);

	printf("\n\n");
	printf("************After Swapping************\n\n");
	printf("Value of 'A' = %d\n\n", a);
	printf("Value of 'B' = %d\n\n", b);

	return(0);

}
void SwapNumbers(int x, int y)
{
	//variable declarations
	int temp;

	printf("\n\n");
	printf("************BEFORE SWAPPING***************\n\n");
	printf("Value of 'X' = %d\n\n", x);
	printf("Value of 'Y' = %d\n\n", y);

	temp = x;
	x = y;
	y = temp;

	printf("\n\n");
	printf("************After Swapping*****************\n\n");
	printf("Value of 'X' = %d\n\n", x);
	printf("Value of 'Y' = %d\n\n", y);
	return(0);
}