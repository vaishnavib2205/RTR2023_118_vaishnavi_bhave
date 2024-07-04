#include <stdio.h>
int main(void) {
	//function declarations
	void MathematicalOperations(int, int*, int*, int*, int*);

	//variable declarations
	int a;
	int b;
	int answer_sum;
	int answer_difference;
	int answer_product;
	int answer_quotient;
	int answer_remainder;

	//code
	printf("\n\n");
	printf("Enter value of 'A' : ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter value of 'B' : ");
	scanf("%d", &b);

	MathematicalOperations(a, b, &answer_sum, &answer_difference, &answer_product, &answer_quotient, &answer_remainder);

	printf("\n\n");
	printf("************************RESULT**********************");
	printf("Sum =%d\n\n", answer_sum);
	printf("Difference =%d\n\n", answer_difference);
	printf("Product = %d\n\n", answer_product);
	printf("Quotient = %d\n\n", answer_quotient);
	printf("Remainder =%d\n\n", answer_remainder);

	return(0);

}
void MathematicalOperations(int x, int y, int *sum, int *difference, int *product, int *quotient, int *remainder)
{
//code
	*sum = x + y;
	*difference = x - y;
	*product = x * y;
	*quotient = x / y;
	*remainder = x % y;
}