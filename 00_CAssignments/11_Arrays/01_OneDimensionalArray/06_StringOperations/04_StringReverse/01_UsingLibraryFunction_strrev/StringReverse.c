#include<stdio.h>

#include<string.h>

#define MAX_STRING_LENGTH 512

int main(void) {
	//variable declarations
	char chArray_Original[MAX_STRING_LENGTH];

	//code
	//String Input
	printf("\n\n");
	printf("Enter a string: \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	//String Output
	printf("\n\n");
	printf("The original String entered by you is: \n\n");
	printf("%s\n", chArray_Original);

	printf("\n\n");
	printf("The reversed string is : \n\n");
	printf("%s/n", strrev(chArray_Original));

	return(0);

}