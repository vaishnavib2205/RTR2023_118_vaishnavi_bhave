#include <stdio.h>
#define MAX_STRING_LENGTH 512

int main(void) {

	//function prototype
	void MyStrcat(char[], char[]);

	//variable declarations
	char chArray_One[MAX_STRING_LENGTH], chArray_Two[MAX_STRING_LENGTH];
	//character array is a string

	//code

	//***string input***
	printf("\n\n");
	printf("Enter first string: \n\n");
	gets_s(chArray_One, MAX_STRING_LENGTH);

	printf("\n\n");
	printf("Enter Second String: \n\n");
	gets_s(chArray_Two, MAX_STRING_LENGTH);

	//***String Concat***
	printf("\n\n");
	printf("**********Before Concatenation*********");
	printf("\n\n");
	printf("The original first string entered by you is: \n\n");
	printf("%s\n", chArray_One);

	printf("\n\n");
	printf("**********After Concatenation*********");
	printf("\n\n");
	printf("The original Second string entered by you is: \n\n");
	printf("%s\n", chArray_Two);

	MyStrcat(chArray_One, chArray_Two);

	printf("\n\n");
	printf("**********Before Concatenation*********");
	printf("\n\n");
	printf("The original first string entered by you is: \n\n");
	printf("%s\n", chArray_One);

	printf("\n\n");
	printf("'chArray_Two[]' is: \n\n");
	printf("%s\n", chArray_Two);

	return(0);
}
	void MyStrcat(char str_destination[], char str_source[])
	{
		//function prototype
		int MyStrlen(char []);

		//variable declarations
		int iStringLength_Source = 0, iStringLength_Destination = 0;
		int i, j;

		//code
		iStringLength_Source = MyStrlen(str_source);
		iStringLength_Destination = MyStrlen(str_destination);

		for (i = iStringLength_Destination, j = 0; j < iStringLength_Source; i++, j++){
			str_destination[i] = str_source[j];
		}

		str_destination[i] = '\0';
	}
		MyStrlen(char str[])
		{
			int j;
			int string_length = 0;

			//code
		
			//Determining exact length of the character of the string by detecting the first occurance of null terminating character(\0) ***
			for (j = 0; j < MAX_STRING_LENGTH; j++)
			{
				if (str[j] == '\0')
					break;
				else
					string_length++;
			}
			return(string_length);

		}
	