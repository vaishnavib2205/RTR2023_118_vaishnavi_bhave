#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	int MyStrlen(char*);
	
	char* chArray = NULL;
	int iStringLength = 0;

	printf("\n\n");
	chArray = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (chArray == NULL)
	{
		printf("MEMORY ALLOCATION TO CHARACTER ARRAY FAILED!!! EXITTING NOW...\n\n");
		exit(0);
	}

	printf("Enter a string: \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	printf("\n\n");
	iStringLength = MyStrlen(chArray);
	printf("Length of string is = %d characters !!!\n\n", iStringLength);

	if (chArray)
	{
		free(chArray);
		chArray = NULL;
	}
	return(0);
}

int MyStrlen(char *str)
{
	int j;
	int string_length = 0;

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (*(str + j) == '\0')
			break;
		else
			string_length++;
	}

	return(string_length);
}