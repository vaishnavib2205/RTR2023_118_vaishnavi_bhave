#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	void MyAddition(int, int);

	int a, b;

	printf("\n\n");
	printf("Enter integer value for 'A': ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter integer value for 'B': ");
	scanf("%d", &b);

	MyAddition(a, b);

	return(0);

}

void MyAddition(int a, int b) {

	int sum;

	sum = a + b;
	printf("\n\n");
	printf("Sum of %d and %d = %d\n\n", a, b, sum);
}