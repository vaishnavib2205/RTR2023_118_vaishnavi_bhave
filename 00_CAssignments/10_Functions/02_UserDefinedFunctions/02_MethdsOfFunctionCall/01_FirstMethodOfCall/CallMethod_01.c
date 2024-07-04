#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

//function prototypes
	void MyAddition(void);
	int MySubtraction(void);
	void MyMultiplication(int, int);

	//variable declarations
	int result_subtraction;
	int a_multiplication, b_multiplication;
	int a_division, b_division, result_division;

	//code

	//***Addition***
	MyAddition(); //function call

	//***Subtraction***
	result_subtraction = MySubtraction();
	printf("\n\n");
	printf("Subtraction yields result = %d\n", result_subtraction);

	//***multiplication***
	printf("\n\n");
	printf("Enter Integer value for 'A' for multiplication: ");
	scanf("%d", &a_multiplication);

	printf("\n\n");
	printf("Enter Integer value for 'B' for multiplication: ");
	scanf("%d", &b_multiplication);

	MyMultiplication(a_multiplication, b_multiplication);//function call

	//***Division***
	printf("\n\n");
	printf("Enter integer value for 'A' for division: ");
	scanf("%d", &a_division);

	printf("\n\n");
	printf("Enter integer value for 'B' division: ");
	scanf("%d", &b_division);

	result_division = MyDivision(a_division, b_division);
	printf("\n\n");

	printf("Division of %d and %d gives = %d(Quotient)\n", a_division, b_division, result_division);
	printf("\n\n");

	return(0);

}

//***function definition of MyAddition()***

void MyAddition(void) {
	//function definition

	//variable declaration: local variables to MyAddition()
	int a, b, sum;

	//code
	printf("\n\n");
	printf("Enter integer value for 'A' for addition: ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter integer value for 'B' for addition: ");
	scanf("%d", &b);

	sum = a + b;
	
	printf("\n\n");
	printf("Sum of %d and %d = %d\n\n", a, b, sum);

}

int MySubtraction(void) {

	int a, b, subtraction;
	printf("\n\n");
	printf("enter integer value for 'A' for subtraction: ");
	scanf("%d", &a);

	printf("\n\n");
	printf("enter integer value for 'B' for subtraction: ");
	scanf("%d", &b);

	subtraction = a - b;
	return(subtraction);

}

void MyMultiplication(int a, int b) {
	int multiplication;

	//code
	multiplication = a * b;

	printf("\n\n");
	printf("Multiplication of %d and %d  = %d\n\n", a, b, multiplication);
}

//***function definition of MyDivision()***
int MyDivision(int a, int b)//function definition
{
	//variable declaration: local variables to mydivision()
	int division_quotient;

	//code
	if (a > b)
		division_quotient = a / b;
	else
		division_quotient = b / a;

	return(division_quotient);
}