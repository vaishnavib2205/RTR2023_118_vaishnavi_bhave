#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	//function prototype
	int MyStrlen(char[]);

	//variable declarations
	char chArray[MAX_STRING_LENGTH];
	int iStringLength;
	int count_A = 0, count_E = 0, count_I = 0, count_O = 0, count_U = 0;

	//initial counts = 0
	int i;

	//code
	//*** String Input ***

	printf("\n\n");
	printf("Enter a string: \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	//*** String Output ***
	printf("\n\n");
	printf("String Entered By you is: \n\n");
	printf("%s\n", chArray);

	iStringLength = MyStrlen(chArray);

	for (i = 0; i < iStringLength; i++)
	{
		switch (chArray[i])
		{
		case 'A' :
		case 'a': 
			count_A++;
			break;

		case 'E':
		case 'e':
			count_E++;
			break;

		case 'I':
		case 'i':
			count_I++;

		case 'O':
		case 'o':
			count_O++;
			break;

		case 'U':
		case 'u':
			count_U++;
			break;

		default:
			break;
		}
	}

	printf("\n\n");
	printf("In the string by you, the vowels and the numbers of their occurances are follows: \n\n");
	printf("'A' has occured = %d times!!!\n\n", count_A);
	printf("'E' has occured = %d times!!!!\n\n", count_E);
	printf("'I' has occured = %d times!!!\n\n", count_I);
	printf("'O' has occured = %d times!!!\n\n", count_O);
	printf("'U' has occured = %d times!!!\n\n", count_U);

	return(0);

}

int MyStrlen(char str[])
{
	int j;
	int string_length = 0;

	//code
	//****Determining exact length of the string by detecting the first occurance of null-terminating character(/0)****

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;

	}

	return(string_length);
}