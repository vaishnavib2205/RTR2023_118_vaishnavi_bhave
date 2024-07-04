#include<stdio.h>

#define MAX_STRING_LENGTH 512

#define SPACE ' '

#define FULLSTOP '.'
#define COMMA ','
#define EXCLAMATION '!'
#define QUESTION_MARK '?'

int main(void)
{
	// function prototyoe
	int MyStrlen(char[]);
	char MyToUpper(char);

	// variable declaration

	char chArray[MAX_STRING_LENGTH], chArray_CapitalizedFirstLetterOfEveryWord[MAX_STRING_LENGTH]; // A Character array is a string

	int iStringLength;
	int i, j;

	// code

		// string input

	printf("\n\n");
	printf("Enter A String : \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	iStringLength = MyStrlen(chArray);

	j = 0;

	for (i = 0; i < iStringLength; i++)
	{
		if (i == 0)
		{
			chArray_CapitalizedFirstLetterOfEveryWord[j] = MyToUpper(chArray[i]);
		}
		else if (chArray[i] == ' ')
		{
			chArray_CapitalizedFirstLetterOfEveryWord[j] = chArray[i];
			chArray_CapitalizedFirstLetterOfEveryWord[j + 1] = MyToUpper(chArray[i + 1]);

			// Since alredy two characters (At Indices 'i' AND 'i+1' have been considered in this else-if block we are extra-incrementing 'i' AND 'j' By 1)

			j++;
			i++;
		}

		else if ((chArray[i] == FULLSTOP || chArray[i] == COMMA || chArray[i] == EXCLAMATION || chArray[i] == QUESTION_MARK) && (chArray[i] != SPACE))
			// first letter of every word after punctuation mark, In the sentence Must be a capital letter
		{
			chArray_CapitalizedFirstLetterOfEveryWord[j] = chArray[i];
			chArray_CapitalizedFirstLetterOfEveryWord[j + 1] = SPACE;
			chArray_CapitalizedFirstLetterOfEveryWord[j + 2] = MyToUpper(chArray[i + 1]);

			// since alredy two characters (At indices 'i' AND i+1 Have Been Considered in this else-if block ... we are extra-icrementing 'i' by 1

			j = j + 2;
			i++;
		}
		else
		{
			chArray_CapitalizedFirstLetterOfEveryWord[j] = '\0';
		}
	}
	// string output

	printf("\n\n");
	printf("String Entered By You Is : \n\n");
	printf("%s\n", chArray);

	printf("\n\n");
	printf("String After Capitalizing First Letter Of Every Word : \n\n ");
	printf("%s\n", chArray_CapitalizedFirstLetterOfEveryWord);

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

char MyToUpper(char ch)
{
	// variable declaration
	int num;
	int c;

	// code

	num = 'a' - 'A';

	if ((int)ch >= 97 && (int)ch <= 122)
	{
		c = (int)ch - num;
		return((char)c);
	}
	else
		return ch;
}