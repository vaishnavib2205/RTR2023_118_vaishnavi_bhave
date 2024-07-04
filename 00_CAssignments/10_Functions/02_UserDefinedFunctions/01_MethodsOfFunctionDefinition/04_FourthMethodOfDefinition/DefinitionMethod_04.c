#include <stdio.h> // stdio.h contains declaration of printf()

// Entry-point function => main() => Valid Retrun Type (int) and 3 Parameters (int argc, char *argv[], char *envp[])

int main(int argc, char* argv[], char* envp[])
{
	//function prototype 
	int MyAddition(int, int);

	// variable declaration : ;local variable to main()

	int a, b;
	int result;
	//code
	printf("\n\n");
	printf("Enter Integer Value For 'A' :");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Integer Value For 'B' :");
	scanf("%d", &b);

	//code
	result = MyAddition(a, b); //function call
	printf("\n\n");
	printf("Sum Of %d And %d = %d \n\n", a, b, result);
	return 0;
}

// user defined function : Method ofdefination 4
// Valid int return value, valid parameters (int ,int)

int MyAddition(int a, int b)//function declarations
{
	// variable declaration : ;local variable to MyAddition()
	int sum;
	sum = a + b;

	return sum;


}