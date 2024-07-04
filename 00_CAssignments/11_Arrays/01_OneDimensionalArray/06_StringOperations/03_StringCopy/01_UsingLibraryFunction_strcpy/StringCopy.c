#include <stdio.h>
#define MAX_STRING_LENGTH 512

int main(void)
{
	//variable declarations
	char chArray_Original[MAX_STRING_LENGTH], chArray_Copy[MAX_STRING_LENGTH];

	//a character array is a string

	//code
	//***String Input***
	printf("\n\n");
	printf("Enter a String: \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	//string copy
	printf("\n\n");
	printf("The original string entered by you: \n\n");
	printf("%s\n", chArray_Original);

	printf("\n\n");
	printf("The copied string(i.e.: 'chArrat_Copy[]')is: \n\n");
	printf("%s\n", chArray_Copy);

	return(0);
}