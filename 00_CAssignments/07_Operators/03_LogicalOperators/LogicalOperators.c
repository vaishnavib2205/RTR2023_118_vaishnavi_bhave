#include <stdio.h>

int main(void)
{
	//variable declaration
	int a;
	int b;
	int c;
	int result;

	//code
	printf("\n\n");
	printf("Enter First Integer : ");
	printf("%d", &a);

	
	printf("\n\n");
	printf("Enter Second Integer : ");
	printf("%d", &b);

	
	printf("\n\n");
	printf("Enter Thrid Integer : ");
	printf("%d", &c);

	printf("\n\n");
	printf("If Answer = 0, it is 'False.\n");
	printf("If Answer = 1, it is 'True'.\n");

	result = (a <= b) && (b != c);
	printf("LOGICAL AND (&&): Answer is True(1) if and only if noth conditions are true. The answer is False(0), if any one or both conditions are false. \n\n");
	printf("LOGICAL AND (&&): Answer is True(1) if and only if noth conditions are true. The answer is False(0), if any one or both conditions are false. \n\n");

	printf("A =%d is less than or equal to B=%d is not equal to C = %d \t Answer = %d\n\n", a, b, b, c, result);

	result = (b >= a) || (a == c);
	printf("Logical OR (||): Answer is false(0) if and only if both conditions are false. The answer is true(1) if any one or both condition are true\n\n");
	printf("Either B =%d is greater than or equal to A = %d OR A = %d is equal to C = %d \t Answer= %d\n\n", b, a, a, c, result);

	result = !a;
	printf("A = %d And using Logical NOT(!) operator on A gives result =%d\n\n", a, result);
}