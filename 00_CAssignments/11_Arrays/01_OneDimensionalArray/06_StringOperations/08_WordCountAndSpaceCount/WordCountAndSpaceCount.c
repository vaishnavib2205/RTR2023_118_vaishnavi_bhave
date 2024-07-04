#include<stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	// function prototype

	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	// variable declaration

	char chArray[MAX_STRING_LENGTH]; // A Character array is a string

	int iStringLength;
	int i;
	int word_count = 0, space_count = 0;

	//code

	// string input

	printf("\n\n");
	printf("Enter A String : \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	iStringLength = MyStrlen(chArray);

	for (i = 0; i < iStringLength; i++)
	{
		switch (chArray[i])
		{
		case 32: // 32 Is The ASCII Value for SPACE ' ' character
			space_count++;
			break;
		default:
			break;
		}
	}

	word_count = space_count + 1;

	// string output

	printf("\n\n");
	printf("String Entered By YOu Is : \n\n");
	printf("%s\n", chArray);

	printf("\n\n");
	printf("Number Of Spaces In The Input string = %d\n\n", space_count);
	printf("Number Of words In The Input string = %d\n\n", word_count);

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