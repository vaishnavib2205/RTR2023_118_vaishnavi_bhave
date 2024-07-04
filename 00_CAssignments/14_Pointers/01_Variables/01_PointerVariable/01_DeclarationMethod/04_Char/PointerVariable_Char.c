#include<stdio.h>

int main(void)
{
	// variable declarations

	char ch;
	char* ptr = NULL; // declaraction method 1 :- '*ptr' is a variable of type 'char' 

	// code
	ch = 'A';

	printf("\n\n");

	printf("***** BEFORE ptr = &ch ****\n\n");
	printf("Value Of 'ch'				= %c\n\n", ch);
	printf("Address Of 'ch'			= %p\n\n", &ch);
	printf("Value At Address Of 'ch'		= %c\n\n", *(&ch));

	//Assigning address of variable 'ch' to pointer variable 'ptr' 
	// ptr now contains address of 'ch' .... hence, ptr is same as &num

	ptr = &ch;

	printf("***** After ptr = &ch ****\n\n");
	printf("Value Of 'ch'				= %c\n\n", ch);
	printf("Address Of 'ch'			= %p\n\n", ptr);
	printf("Value At Address Of 'ch'		= %c\n\n", *ptr);

	return 0;
}