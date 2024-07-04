#include<stdio.h>
#include<stdlib.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	// function prototype
	void MyStrcat(char*, char*);
	int MyStrlen(char*);

	//variable declaration

	char* chArray_one = NULL, * chArray_Two = NULL;

	// code

	printf("\n\n");
	chArray_one = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (chArray_one == NULL)
	{
		printf("MEMORY ALLOCATION TO FIRST STRING FAILED !!! EXITING NOW...\n\n");
		exit(0);
	}

	printf("Enter First String : \n\n");
	gets_s(chArray_one, MAX_STRING_LENGTH);

	printf("\n\n");
	chArray_Two = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (chArray_Two == NULL)
	{
		printf("MEMORY ALLOCATION TO SECOND STRING FAILED !!! EXITING NOW...\n\n");
		exit(0);
	}

	printf("Enter First String : \n\n");
	gets_s(chArray_Two, MAX_STRING_LENGTH);

	// *** STRING OUTPUT ***

	printf("\n\n");
	printf(" The First String Entered By You is : \n\n");
	printf("%s\n", chArray_one);

	printf("\n\n");
	printf(" The Second String Entered By You is : \n\n");
	printf("%s\n", chArray_Two);

	MyStrcat(chArray_one, chArray_Two);
	printf("\n\n");

	printf("***** AFTER CONCATINATION *****");
	printf("\n\n");
	printf("'chArray_One[]' Is : \n\n");
	printf("%s\n", chArray_one);

	printf("\n\n");
	printf("'chArray_Two[]' Is : \n\n");
	printf("%s\n", chArray_Two);

	if (chArray_Two)
	{
		free(chArray_Two);
		chArray_Two = NULL;
		printf("\n\n");
		printf("MEMORY ALLOCATION TO SECOND STRING HAS BEEN SUCCESSFULLY FREED !!!\n\n");
	}

	if (chArray_one)
	{
		free(chArray_one);
		chArray_one = NULL;
		printf("\n\n");
		printf("MEMORY ALLOCATION TO FIRST STRING HAS BEEN SUCCESSFULLY FREED !!!\n\n");
	}

	return 0;
}

void MyStrcat(char *str_destination, char *str_source)
{
	//function prototype
	int MyStrlen(char*);

	//variable declaration

	int iStringLength_Source = 0, iStringLength_Destination = 0;

	int i, j;

	// code

	iStringLength_Source = MyStrlen(str_source);
	iStringLength_Destination = MyStrlen(str_destination);

	// array indices begin from 0, hence, last valid index of array will always be (Length-1)
	//so, concatination ,must begin from index number equal to length of the array 'str_desrtination'
	// we need to put the character which is at first index of 'str_source' to the (last index +1) of  'str_destination'

	for (i = iStringLength_Destination, j = 0; j < iStringLength_Source; i++, j++)
	{
		*(str_destination + i) = *(str_source + j);
	}

	*(str_destination + i) = '\0';

}

int MyStrlen(char *str)
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