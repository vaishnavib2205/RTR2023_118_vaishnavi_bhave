#include<stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	//function prototype


	//variable declaration

	// *** A String is an array of characters .... so char[] ia a char array and hence, char[] is a String ***
	// *** An Array of char arrays is an array of strings !! *** 
	// ***  HENCE, char[] IS ONE char  ARRRAY AND HENCE , IS ONE STRING ***
	// *** HENCE, CHAR[[][] IS AN ARRAY OF CHAR ARRAYS AND HENCE IT IS AN ARRAY OF STRINGS***

	//Here,  the string array can allow a maximum number of 5 strings(5 row) and each of these 5 strings can have only upto 10 characters maximum(10 column)

	char strArray[5][10]; // 5 ROWS (0,1,2,3,4) -> 5 Strings (EACH STRING CAN HAVE MAXIMUM OF 10 CHARACTERS)

	int char_size;
	int strArray_size;
	int strArray_num_elements, strArray_num_rows, strArray_num_columns;
	int i;

	// code


	printf("\n\n");

	char_size = sizeof(char);

	strArray_size = sizeof(strArray);
	printf("Size of two dimensional (2D) Character Array(String Array) Is = %d\n\n", strArray_size);

	strArray_num_rows = strArray_size / sizeof(strArray[0]);
	printf("Number of Rows in two dimensional (2D) Character Array(String Array) = %d\n\n", strArray_num_rows);

	strArray_num_columns = sizeof(strArray[0]) / char_size;
	printf("Number of Column in two dimensional (2D) Character Array(String Array) Is = %d\n\n", strArray_num_columns);

	strArray_num_elements = strArray_num_rows * strArray_num_columns;
	printf("Maximum Number of Elements in two dimensional (2D) Character Array(String Array) Is = %d\n\n", strArray_num_elements);

	// **** PIECE MEAL ASSIGNMENT ****
	// ***** ROW 1 / STRING 1 *****
	strArray[0][0] = 'M';
	strArray[0][1] = 'Y';
	strArray[0][2] = '\0'; // NULL-TERMINATING CHARACTER 

	// ***** ROW 2 / STRING 2 *****
	strArray[1][0] = 'N';
	strArray[1][1] = 'A';
	strArray[1][2] = 'M';
	strArray[1][3] = 'E';
	strArray[1][4] = '\0'; // NULL-TERMINATING CHARACTER 

	// ***** ROW 3 / STRING 3 *****
	strArray[2][0] = 'I';
	strArray[2][1] = 's';
	strArray[2][2] = '\0'; // NULL-TERMINATING CHARACTER 

	// ***** ROW 4 / STRING 4 *****
	strArray[3][0] = 'V';
	strArray[3][1] = 'A';
	strArray[3][2] = 'I';
	strArray[3][3] = 'S';
	strArray[3][4] = 'H';
	strArray[3][6] = 'N';
	strArray[3][7] = 'A';
	strArray[3][8] = 'V';
	strArray[3][9] = 'I';
	strArray[3][10] = '\0'; // NULL-TERMINATING CHARACTER 

	// ***** ROW 5 / STRING 5 *****
	strArray[4][0] = 'B';
	strArray[4][1] = 'H';
	strArray[4][2] = 'A';
	strArray[4][3] = 'V';
	strArray[4][4] = 'E';
	strArray[4][5] = '\0'; // NULL-TERMINATING CHARACTER 

	printf("\n\n");
	printf(" The Strings In The 2D Character Array Are : \n\n");

	for (i = 0; i < strArray_num_rows; i++)
	{
		printf("%s ", strArray[i]);
	}

	printf("\n\n");

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