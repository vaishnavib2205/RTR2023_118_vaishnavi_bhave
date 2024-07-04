#include <stdio.h>

int main(void)
{
	//variabke declaration 
	int a;
	int b;
	int result;

	//code
	printf("\n\n");
	printf("Enter One Integer: ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Another Integer: ");
	scanf("%d", &b);

	printf("\n\n");
	printf("If Answer = 0, It is 'False',\n");
	printf("If Answer = 1, It is 'True',\n\n");

	result = (a < b);
	printf("(a<b) A = %d is less than B =%d       \t Answer = %d\n", a, b, result);

	result = (a > b);
	printf("(a>b) A = %d is greater than B =%d       \t Answer = %d\n", a, b, result);

	result = (a <= b);
	printf("(a<=b) A = %d is less or equal to B =%d       \t Answer = %d\n", a, b, result);

	result = (a >= b);
	printf("(a>=b) A = %d is greater than or equal to B =%d       \t Answer = %d\n", a, b, result);

	result = (a == b);
	printf("(a==b) A = %d is equal to B =%d       \t Answer = %d\n", a, b, result);

	result = (a != b);

	printf("(a!=b) A = %d is not equal to B =%d       \t Answer = %d\n", a, b, result);

	return(0);
}