#include<stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	//function prototype
	int MyStrlen(char[]);

	// variable Declaration
	char chArray[MAX_STRING_LENGTH]; // Character array is a string
	int iStringLength = 0;

	//code 

	// String Input

	printf("\n\n");
	printf("Enter A String : \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	// String Output
	printf("\n\n");
	printf("String ENtered Bu You Is : \n\n");
	printf("%s\n", chArray);

	// String Length
	printf("\n\n");
	iStringLength = MyStrlen(chArray);
	printf("Length Of String Is = %d Characters !!!\n\n", iStringLength);

	return 0;
}

int MyStrlen(char str[])
{
	// variable declaration
	int j;
	int string_length = 0;

	//code
	// DETERMINING EXACT LENGTH OF THE STRING BY DETECTING THE FIRST OCCURANCE OF THE NULL-TERMINATING CHARACTER (\0)

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
		{
			break;
		}
		else
		{
			string_length++;
		}
	}
	return(string_length);
}