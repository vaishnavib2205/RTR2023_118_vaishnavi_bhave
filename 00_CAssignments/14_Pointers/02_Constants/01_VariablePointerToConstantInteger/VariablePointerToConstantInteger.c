#include<stdio.h>

int main(void)
{
	// variable declarations

	int num = 5;
	const int* ptr = NULL; // Read this as ptr is a pointer to integer constant

	// code

	ptr = &num;
	printf("\n");
	printf("Current Value Of 'num' = %d\n", num);
	printf("Current 'ptr' (Address of num) = %p\n", ptr);

 // the following value does not give anyu error as we are going to modify thew value of the variable individually

	num++;
	printf("\n\n");
	printf("After num++, value of num = %d\n", num);

	// the following line gives error and hence we commented it out
	// we cannot alter the value stored in A pointer to constant integer
	// with respect to the pointer trhe value it points to should be constant 
	
	// (*ptr)++

	// the following line does not give any error
	// we do not give error because we are changing the pointer 
	// the pointer is not constant trhe value to which the pointer points is constant 

	ptr++;

	printf("\n\n");
	printf("After ptr++ value of 'ptr' = %p\n", ptr);
	printf("Value at this new 'ptr' = %d\n", *ptr);
	return 0;

}