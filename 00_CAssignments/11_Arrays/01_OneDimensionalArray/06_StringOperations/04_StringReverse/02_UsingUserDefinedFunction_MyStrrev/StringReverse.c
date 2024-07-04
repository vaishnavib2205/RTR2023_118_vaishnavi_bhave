#include <stdio.h>
#define MAX_STRING_LENGTH 512

int main(void) {

//funtion prototype
	void MyStrrev(char[], char[]);
	
	//variable declarations
	char chArray_Original[MAX_STRING_LENGTH], chArray_Reversed
		[MAX_STRING_LENGTH]; // a character array is a string
		

		//code
		
		//***String Input***]
	printf("\n\n");
	printf("Enter a string: \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	//***String Reverse***
	MyStrrev(chArray_Reversed, chArray_Original);
	
	//***String Output***
	printf("\n\n");
	printf("The original string entered by you is: \n\n");
	printf("%s\n", chArray_Reversed);

	return(0);
}

void MyStrrev(char str_destination[], char str_source[]) {

//function prototype
	int MyStrlen(char[]);
	//variable declarations
	int iStringLength = 0;
	int i, j, len;

	//code
	iStringLength = MyStrlen(str_source);

	//array indices begin from 0, hence last index will always be
	len = iStringLength = 1;

	//we need to put the character which is at least index of 'str_source' to the first index of 'str_source' to the second character of 'str_destination'so on

	for (i = 0, j = len; i < iStringLength, j >= 0; i++, j--) {
		str_destination[i] = str_source[j];
	}
	str_destination[i] = '\0';
}
int MyStrlen(char str[])
{
	//variable declaration
	int j;
	int string_length = 0;
	

	//code 
	//***Determiningnexact length of the string by detecting the first occurance  of null terminating character(\0)****
	for (j = 0; j < MAX_STRING_LENGTH; j++) {
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}

	return(string_length);
}