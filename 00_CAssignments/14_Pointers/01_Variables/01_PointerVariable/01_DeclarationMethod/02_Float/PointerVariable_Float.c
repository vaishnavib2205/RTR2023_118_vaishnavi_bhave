#include<stdio.h>

int main(void)
{
	// variable declarations

	float num;
	float* ptr = NULL; // declaraction method 1 :- '*ptr' is a variable of type 'float' 

	// code
	num = 6.9f;

	printf("\n\n");

	printf("***** BEFORE ptr = &num ****\n\n");
	printf("Value Of 'num'				= %f\n\n", num);
	printf("Address Of 'num'			= %p\n\n", &num);
	printf("Value At Address Of 'num'		= %f\n\n", *(&num));

	//Assigning address of variable 'num' to pointer variable 'ptr' 
	// ptr now contains address of 'num' .... hence, ptr is same as &num

	ptr = &num;

	printf("***** After ptr = &num ****\n\n");
	printf("Value Of 'num'				= %f\n\n", num);
	printf("Address Of 'num'			= %p\n\n", ptr);
	printf("Value At Address Of 'num'		= %f\n\n", *ptr);

	return 0;
}