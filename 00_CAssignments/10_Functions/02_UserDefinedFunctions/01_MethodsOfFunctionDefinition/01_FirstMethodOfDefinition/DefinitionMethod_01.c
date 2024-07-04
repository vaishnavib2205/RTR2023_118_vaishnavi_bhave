#include <stdio.h> //stdio.h contains declaration of printf

//entry point function => main() => valid return type (int) and three parameters (int argc, char *argv[], char *envp[])

int main(int argc, char* argv[], char* envp[]) {
	//function declaration/prototype/signature
	void MyAddition(void);

	//code
	MyAddition();
	return(0);
}
//****User defined function : method of definition 1 ****
//****No return value, no parameters****

void MyAddition(void)//function definition
{
	//variable declarations: local variables to MyAddition()
	int a, b, sum;

	//code
	printf("\n\n");
	printf("Enter integer value for 'A': ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter integer value for 'B': ");
	scanf("%d", &b);

	sum = a + b;

	printf("\n\n");
	printf("Sum of %d and %d = %d\n\n", a, b, sum);
}