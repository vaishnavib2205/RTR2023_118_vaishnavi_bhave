#include<stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	//function prototype
	int MyStrlen(char[]);

	//variable declaration

	// *** A String is an array of characters .... so char[] ia a char array and hence, char[] is a String ***
	// *** An Array of char arrays is an array of strings !! *** 
	// ***  HENCE, char[] IS ONE char  ARRRAY AND HENCE , IS ONE STRING ***
	// *** HENCE, CHAR[[][] IS AN ARRAY OF CHAR ARRAYS AND HENCE IT IS AN ARRAY OF STRINGS***

	//Here,  the string array can allow a maximum number of 10 strings(10 row) and each of these 10 strings can have only upto 15 characters maximum(15 column)

	char strArray[10][15] = { "Hello!", "Welcome", "To" , "Real" , "Time" , "Rendering", "Batch" , "(2023-24)" , "Of" , "ASTROMEDICOMP." }; // IN-LINE INITIALIZATION

	int iStrLength[10]; // 1D Integer Array - stores lengths of those strings at corresponding indices in 'strArray[]' e.g: iStrLength[0] will be the length of string at strArray[0] , iStrLength[1] will be the length of string at strArray[1]....10 strings, 10 Lengths... 

	int strArray_size;
	int strArray_num_rows;

	int i, j;

	// code

	strArray_size = sizeof(strArray);
	strArray_num_rows = strArray_size / sizeof(strArray[0]);

	//sorting in length of all the strings...
	for (i = 0; i < strArray_num_rows; i++)
	{
		iStrLength[i] = MyStrlen(strArray[i]);
	}

	printf("\n\n");
	printf("The Entire String Array : \n\n");
	for (i = 0; i < strArray_num_rows; i++)
	{
		printf("%s ", strArray[i]);
	}

	// Since char[][] is an array of strings, refrencing only by the row number (first[]) will give the row of the string
	// the column number(second[]) is the particular character in that string/row

	for (i = 0; i < strArray_num_rows; i++)
	{
		printf("String Number %d = > %s \n\n", (i + 1), strArray[i]);
		for (j = 0; j < iStrLength[i]; j++)
		{
			printf("Character %d = %c\n", (j + 1), strArray[i][j]);
		}
		printf("\n\n");
	}

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