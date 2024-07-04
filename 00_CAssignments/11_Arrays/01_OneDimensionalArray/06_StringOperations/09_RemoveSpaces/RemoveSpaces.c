#include<stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	// function prototype

	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	// variable declaration

	char chArray[MAX_STRING_LENGTH], chArray_SpaceRemoved[MAX_STRING_LENGTH]; // A Character array is a string

	int iStringLength;
	int i, j;


	//code

	// string input

	printf("\n\n");
	printf("Enter A String : \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	iStringLength = MyStrlen(chArray);
	j = 0;
	for (i = 0; i < iStringLength; i++)
	{
		if (chArray[i] == ' ')
			continue;
		else
		{
			chArray_SpaceRemoved[j] = chArray[i];
			j++;
		}
	}

	chArray_SpaceRemoved[j] = '\0';

	// string output

	printf("\n\n");
	printf("String Entered By You Is : \n\n");
	printf("%s\n", chArray);

	printf("\n\n");
	printf("String After Removal Of Spaces Is : \n\n ");
	printf("%s\n", chArray_SpaceRemoved);

	return 0;

}


int MyStrlen(char str[])
{
	// variable declaration
	int j;
	int string_length = 0;
	//code

	//*** Determining exact length of the string by detecting the first occurence of null-terminationg character ( \0 ) ***

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

		return string_length;
	}

}