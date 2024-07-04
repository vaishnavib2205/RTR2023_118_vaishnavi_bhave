#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	void MathematicalOperations(int, int, int*, int*, int*, int*, int*);
	int a;
	int b;
	int *answer_sum = NULL;
	int *answer_difference = NULL;
	int *answer_product = NULL ;
	int *answer_quotient = NULL;
	int *answer_remainder = NULL;

	printf("\n\n");
	printf("Enter value of 'A' : ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter value of 'B' : ");
	scanf("%d", &b);


	answer_sum = (int*)malloc(1 * sizeof(int));
	if (answer_sum = NULL)
	{
		printf("Could not allocate memory for 'answer_sum'\n\n");
		exit(0);

	}
	answer_difference = (int*)malloc(1 * sizeof(int));
	if (answer_difference == NULL)
	{
		printf("Could not allocate memory for 'answer_difference' exitting now!!!\n\n");
		exit(0);
	}
	answer_product = (int*)malloc(1 * sizeof(int));
	if (answer_product == NULL)
	{
		printf("Could not allocate memory for 'answer_product' exitting now....\n\n");
		exit(0);
	}

	answer_quotient = (int*)malloc(1 * sizeof(int));
	if (answer_quotient == NULL)
	{
		printf("Could not allocate memory for answer_quotient. Exitting now...\n\n");
		exit(0);
	}

	answer_remainder = (int*)malloc(1 * sizeof(int));
	if (answer_remainder == NULL)
	{
		printf("Could not allocate memory for answer_remainder. Exitting now...\n\n");
		exit(0);
	}

	MathematicalOperations(a, b, answer_sum, answer_difference, answer_product, answer_quotient, answer_remainder);

	printf("\n\n");
	printf("************************RESULTS**********************\n\n");
	printf("Sum =%d\n\n", *answer_sum);
	printf("Difference =%d\n\n", *answer_difference);
	printf("Product = %d\n\n", *answer_product);
	printf("Quotient = %d\n\n", *answer_quotient);
	printf("Remainder =%d\n\n", *answer_remainder);

	if (answer_remainder)
	{
		free(answer_remainder);
		answer_quotient = NULL;
		printf("Memory allocated for answer_remainder' successfully freed!!!\n\n");

	}

	if (answer_product)
	{
		free(answer_product);
		answer_product = NULL;
		printf("Memory allocated for answer_product' successfully freed!!!\n\n");

	}

	if (answer_difference)
	{
		free(answer_difference);
		answer_difference = NULL;
		printf("Memory allocated for answer_difference' successfully freed!!!\n\n");

	}

	if (answer_sum)
	{
		free(answer_sum);
		answer_sum = NULL;
		printf("Memory allocated for answer_sum' successfully freed!!!\n\n");

	}

	return(0);
}

void MathematicalOperations(int x, int y, int* sum, int* difference, int* product, int* quotient, int* remainder)
{
	*sum = x + y;
	*difference = x - y;
	*product = x * y;
	*quotient = x / y;
	*remainder = x % y;
}
