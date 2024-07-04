//  This program replaces all vowles in the input string with the * symbol

// for example orignal string 'Prasad Bhalkikar' Will Become 'Pr*s*d *h*lk*k*r

#include<stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	// function prototype
	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	char chArray_Original[MAX_STRING_LENGTH], chArray_VowelReplaced[MAX_STRING_LENGTH];

	int iStringLength;
	int i;

	// code

		// String Input

	printf("\n\n");
	printf("Enter A String : \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	// String Output

	MyStrcpy(chArray_VowelReplaced, chArray_Original);

	iStringLength = MyStrlen(chArray_VowelReplaced);

	for (i = 0; i < iStringLength; i++)
	{
		switch (chArray_VowelReplaced[i])
		{
		case 'A':
		case 'a':
		case 'E':
		case 'e':
		case 'I':
		case 'i':
		case 'O':
		case 'o':
		case 'U':
		case 'u':
			chArray_VowelReplaced[i] = '*';
			break;
		default:
			break;
		}
	}

	// String Output
	printf("\n\n");
	printf("The Original String Entered By You (i.e 'chArray_Original][]') Is: \n\n");
	printf("%s\n", chArray_Original);

	printf("\n\n");
	printf("String After Replacement Of Vowles By * Is : \n\n");
	printf("%s\n", chArray_VowelReplaced);

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


void MyStrcpy(char str_destination[], char str_source[])
{
	// function prototype
	int MyStrlen(char[]);

	//variable declaration
	int iStringLength = 0;
	int j;

	// code
	iStringLength = MyStrlen(str_source);
	for (j = 0; j < iStringLength; j++)
	{
		str_destination[j] = str_source[j];
	}
	str_destination[j] = '\0';
}

