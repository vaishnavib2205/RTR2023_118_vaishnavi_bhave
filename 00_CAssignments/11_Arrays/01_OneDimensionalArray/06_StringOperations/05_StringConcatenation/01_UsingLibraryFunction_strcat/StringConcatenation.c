#include<stdio.h>
#define MAX_STRING_LENGTH 512
int main(void) {

//variable declaration
	char chArray_One[MAX_STRING_LENGTH], chArray_Two[MAX_STRING_LENGTH];
	//character array is a string
	//code
	//***String Input***
	printf("\n\n");
	printf("Enter first string: \n\n");
	gets_s(chArray_One, MAX_STRING_LENGTH);

	printf("\n\n");
	printf("Enter Second string: \n\n");
	gets_s(chArray_Two, MAX_STRING_LENGTH);

	//***String Concat***
	printf("\n\n");
	printf("**********Before Concatenation*********");
	printf("\n\n");
	printf("The original first string entered by you is: \n\n");
	printf("%s\n", chArray_One);

	printf("\n\n");
	printf("**********After Concatenation*********");
	printf("\n\n");
	printf("The original Second string entered by you is: \n\n");
	printf("%s\n", chArray_Two);

	strcat(chArray_One, chArray_Two);

	printf("\n\n");
	printf("'chArray_Two[]' is: \n\n");
	printf("%s\n", chArray_Two);

	return(0);
}