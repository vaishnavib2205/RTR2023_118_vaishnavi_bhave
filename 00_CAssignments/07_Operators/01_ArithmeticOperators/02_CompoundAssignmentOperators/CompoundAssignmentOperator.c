#include <stdio.h>

int main(void)
{
	//variable declaration
	int a;
	int b;
	int x;

	//code
	printf("\n\n");
	printf("Enter A Number: ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Another Number: ");
	scanf("%d", &b);

	printf("\n\n");

	//since in all the following 5 cases the operand on the left 'a' is getting repeated immediately on the right (e.g : a + b or a = a - b),
	//we are using compound assignment operators +=, -+, *+, /+ and %=

	//since 'a' will be assigned the value of (a+b) at the expression (a += b) we msut save the original value of 'a' to another variable (x)
	x = a;
	a += b;
	printf("Addition of A = %d and B = %d gives %d. \n", x, a, b);

	//value of 'a' altered in the above expression is used here...
	//since 'a' will be assigned the value of (a - b) at the expression (a -= b), we must the original variable(x)
	x = a;
	a -= b;
	printf("Subtraction of A = %d and B = %d gives %d. \n", x, a, b);

	//value of 'a' altered in the above expression is used here...
	//since 'a' will be assigned the value of (a*b) at the expression (a*=b), we must save the original value of 'a' to another variable (x)
	x = a;
	a *= b;
	printf("Multiplication of A = %d and B = %d Gives %d \n", x, a, b);

	//value of 'a' altered in the above expression is used here...
	//since 'a' will be assigned the value of (a/b) at the expression (a/=b) we must save the original value of 'a' to another variable(x)

	x = a;
	a /= b;
	printf("Division of A = %d and B = %d gives  quotient %d \n", x, a, b);
	//value of 'a' altered in the above expression is used here...
	//since 'a' will be assigned the value of (a % b) at the expression (a%=b), we must save the original value of 'a' to another variable (x)

	x = a;
	a %= b; //a = a %b

	printf("Division of A = %d and B = %d gives remainder %d \n", x, a, b);

	printf("\n\n");
	return(0);
}