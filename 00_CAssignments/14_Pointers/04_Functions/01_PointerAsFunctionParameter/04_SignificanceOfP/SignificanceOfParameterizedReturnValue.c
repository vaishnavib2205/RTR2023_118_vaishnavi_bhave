#include <stdio.h>

enum {
	NEGATIVE = -1,
	ZERO,
	POSITIVE
};

int main(void) {
	//function declarations
	int difference(int, int, int*);

	//variable declarations
	int a;
	int b;
	int answer, ret;

	//code
	printf("\n\n");
	printf("Enter value of A: ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter value of B: ");
	scanf("%d", &b);

	ret = difference(a, b, &answer);

	printf("\n\n");
	printf("The difference %d and %d is positive !!!\n\n", a, b, answer);

	if (ret == POSITIVE)
		printf("The difference of %d and %d is positive!!!\n\n", a, b);
	else if (ret == NEGATIVE)
		printf("The difference of %d and %d is Negative!!!\n\n", a, b);
	else
		printf("The difference of %d and %d is zero!!!\n\n", a, b);

	return(0);
}
int difference(int x, int y, int *diff) {
//code'
	*diff = x - y;
	if (*diff > 0)
		return(POSITIVE);
	else if (*diff < 0)
		return(NEGATIVE);
	else
		return(ZERO);
}