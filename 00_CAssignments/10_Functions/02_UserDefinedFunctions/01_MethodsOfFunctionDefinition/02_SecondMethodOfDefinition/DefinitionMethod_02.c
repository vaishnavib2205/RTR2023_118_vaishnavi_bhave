#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
	// function prototype/ declaration / signature
	int MyAddition(void);

	int result;
	result = MyAddition(); //function call


	printf("\n\n");
	printf("Sum = %d\n\n", result);
	return(0);
}
	//**** User defined function : method of definition 2 ****
	//***** valid (int) return value, no parameters *****

	int MyAddition(void) //function declaration
	{
	//variable declarations : local variables to MyAddition()
		int a, b, sum;

		//code
		printf("\n\n");
		printf("Enter Integer value for 'A': ");
		scanf("%d", &a);

		printf("\n\n");
		printf("Enter integer value for 'B': ");
		scanf("%d", &b);

		sum = a + b;

		return(sum);
	}
