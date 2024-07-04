#include <stdio.h>

int main(void)
{
	//variable declaration
	int a;
	int b;
	int result;

	//code
	printf("\n\n");
	printf("Enter A Number: ");
	scanf("%d",&a);

	printf("\n\n");
	printf("Enter Another Number : ");
	scanf("%d", &b);
	printf("\n\n");
	result = a + b;
	printf("Addition Of A = %d And B = %d Gives %d. \n", a, b, result);

	result = a - b;
	printf("Suntraction And B = %d Gives %d. \n", a, b, result);

	result = a * b;
	printf("Multiplication of = %d Gives %d. \n", a, b, result);

	result = a / b;
	printf("Division %d And B = %d Gives Quotient %d.\n", a, b, result);

	result = a % b;
	printf("Division Of A = %d And B = %d Gives Remainder %d\n", a, b, result);

	printf("\n\n");

	return(0);
}