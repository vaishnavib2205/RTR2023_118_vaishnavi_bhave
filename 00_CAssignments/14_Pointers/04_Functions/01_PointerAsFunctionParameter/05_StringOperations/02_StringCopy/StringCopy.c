#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	//function prototype
	void MyStrcpy(char*, char*);
	int MyStrlen(char*);

	char* chArray_Original = NULL, * chArray_Copy = NULL;

	int original_string_length;

	//code
	printf("\n\n");
	chArray_Original = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (chArray_Original == NULL)
	{
		printf("MEMORY ALLOCATION FOR ORIGINAL STRING FAILED!!! EXITTING NOW...\n\n");
		exit(0);
	}
	printf("Enter a string: \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	original_string_length = MyStrlen(chArray_Original);
	chArray_Copy = (char*)malloc(original_string_length * sizeof(char));
	if (chArray_Copy == NULL)
	{
		printf("MEMORY ALLOCATION FOR COPIED STRING FAILED!!! EXITTING NOW...\n\n");
		exit(0);
	}

	MyStrcpy(chArray_Copy, chArray_Original);

	printf("\n\n");
	printf("The original string entered by yu : \n\n");
	printf("%s\n", chArray_Copy);

	if (chArray_Copy)
	{
		free(chArray_Copy);
		chArray_Copy = NULL;
		printf("\n\n");
		printf("MEMORY ALLOCATED FOR COPIED STRING HAS BEEN SUCCESSFULLY FREED!!!\n\n");
	}

	if (chArray_Original)
	{
		free(chArray_Original);
		chArray_Original = NULL;
		printf("\n\n");
		printf("MEMORY ALLOCATED FOR ORIGINA; STRING HAS BEEN SUCCESSFULLY FREED !!!\n\n");
	}
	return 0;
}


void MyStrcpy(char* str_destination, char* str_source)
{
	int MyStrlen(char*);
	int iStringLength = 0;
	int j;

	iStringLength = MyStrlen(str_source);
	for (j = 0; j < iStringLength; j++)
		*(str_destination + j) = *(str_source + j);

	*(str_destination + j) = '\0';
}

int MyStrlen(char* str)
{
	int j;
	int String_length = 0;
	int string_length = 0;

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);
}

