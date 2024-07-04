#include <stdio.h>
int main(void)
{
	//variable declarations
	int a, b, p;
	//code
	a = 9;
	b = 30;
	p = 30;

	// *** First if-else Pair ***

	printf("\n\n");

	if (a < b) {
		printf("ENtering first if block...\n\n");
		printf("A is less than B!!!\n\n");
	}
	else {
		printf("Entering first else block...\n\n");
		printf("A is not less than B \n\n");
	}
	printf("First if-else pair done!\n\n");
	//***second if else pair***

	printf("\n\n");
	if (b != p) {
		printf("Entering second if block ...\n\n");
		printf("B is not equal to P!!!\n\n");
	}
	else {
		printf("Entering Second else block...\n\n");
		printf("B is equal to P!!!\n\n");
	}
	printf("Second if else pair done!!!\n\n");

	return(0);
}