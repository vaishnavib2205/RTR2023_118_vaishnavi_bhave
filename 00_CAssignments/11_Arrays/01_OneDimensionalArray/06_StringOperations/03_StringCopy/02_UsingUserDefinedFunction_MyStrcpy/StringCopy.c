#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void) {
	//function prototype
	void MyStrcpy(char[], char[]);

	//variable declarations
	char chArray_Original[MAX_STRING_LENGTH], chArray_Copy[MAX_STRING_LENGTH];
	//a character array is a string

	//code
	//***String Input***
	printf("\n\n");
	printf("Enter a string: \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	//***String Copy***
	MyStrcpy(chArray_Copy, chArray_Original);

	//***String Output***
	printf("\n\n");
	printf("The original string entered by you(i.e.: 'chArray_Original[]') is: \n\n");
	printf("%s\n", chArray_Copy);

	return(0);

}
void MyStrcpy(char str_destination[], char str_source[]) {

//function prototype
	int MyStrlen(char[]);
	//variable declarations 
	int iStringLength = 0;
	int j;

	//code
	iStringLength = MyStrlen(str_source);
	for (j = 0; j < iStringLength; j++)
		str_destination = str_source;

	str_destination[j] = '\0';
}

int MyStrlen(char str[]) {

//variable declarations
	int j;
	int string_length = 0;

	//code
	//***Determining Exact length of the string by detecting the first occurance of NUll trminating character(\0)***
	for (j = 0; j < MAX_STRING_LENGTH; j++) {
		if (str[j] == '\0')
			break;
		else
			string_length++;

	}
	
	return(string_length);
}