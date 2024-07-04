#include<stdio.h>

#define MAX_STRING_LENGTH 512

int main(void) {
	//variable declarations
	char chArray[MAX_STRING_LENGTH]; //a character array is A STRING

	//code

	printf("\n\n");
	printf("Enter a string: \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	printf("\n\n");
	printf("String Entered Bu you is : \n\n");
	printf("%s\n", chArray);
	return(0);  
}