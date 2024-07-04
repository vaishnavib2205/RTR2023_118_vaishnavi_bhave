#include <stdio.h>

struct Employee {
	char name[100];
	int age;
	float salary;
	char sex;
	char material_status;
};

int main(void) {
	//code
	printf("\n\n");
	printf("Sizes of data types and pointers t those respective data types are: \n\n");
	printf("size of (int)         : %d   \t  \t  \t size of pointer t int (*int)        : %d  \t  \t size of pointer to pointer: %d\n\n", sizeof(int), sizeof(int*), sizeof(int**));
	printf("size of (float)         : %f   \t  \t  \t size of pointer t float (*float)        : %d  \t  \t size of pointer to pointer: %d\n\n", sizeof(float), sizeof(float*), sizeof(float**));  
	printf("size of (double)         : %lf   \t  \t  \t size of pointer t double (*double)        : %d  \t  \t size of pointer to pointer: %d\n\n", sizeof(double), sizeof(double*), sizeof(double**));
	printf("size of (char)         : %c   \t  \t  \t size of pointer t char (*char)        : %d  \t  \t size of pointer to pointer: %d\n\n", sizeof(char), sizeof(char*), sizeof(char**));
	printf("size of (struct employee)         : %d   \t  \t  \t size of pointer t struct employee (*struct employee)        : %d  \t  \t size of pointer to pointer: %d\n\n", sizeof(struct employee), sizeof(struct employee*), sizeof(struct employee**));

	return(0);
}