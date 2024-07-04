#include <stdio.h> //for printf()
#include <conio.h>//for getch

// #define macros sathi use kartat
/* These macro definitions allow constant values to be declared for use throughout your code. Macro definitions are not variables 
and cannot be changed by your program code like variables.*/

//ASCII values for 'A' to 'Z' => 65 to 90
#define CHAR_ALPHABET_UPPER_CASE_BEGINNING 65
//check whether the character is in alphabetical characters or not in the beginning
#define CHAR_ALPHABET_UPPER_CASE_ENDING 90
//check whether the character is in alphabetical characters or not in the ending


//ASCII values for 'a' to 'z' => 97 to 122
#define CHAR_ALPHABET_LOWER_CASE_BEGINNING 97
#define CHAR_ALPHABET_LOWER_CASE_ENDING 122


//ASCII values for '0' to '9'
#define CHAR_DIGIT_BEGINNING 48
#define CHAR_DIGIT_ENDING 57

int main(void)
{
	//variable declarations
	char ch;
	int ch_value;

	//code
	printf("\n\n");

	printf("Enter a character: ");
	ch = getch();

	printf("\n\n");

	switch (ch)
	{
		//fall through condition...
	case 'A' :
	case 'a':

	case 'E':
	case 'e':

	case 'I':
	case 'i':

	case 'O':
	case 'o':

	case 'U':
	case 'u':

		printf("Character \'%c\' entered by you, is a vowel character from the english alphabet!!!\n\n", ch);
		break;
	default:
		ch_value = (int)ch;

		//if the character has ascii value between 65 and 90 or between 97 and 122 it is still a letter of the alphabet but it is a consonant and not a vowel...

		if ((ch_value >= CHAR_ALPHABET_UPPER_CASE_BEGINNING && ch_value <= CHAR_ALPHABET_UPPER_CASE_ENDING) || (ch_value <= CHAR_ALPHABET_LOWER_CASE_BEGINNING && ch_value
			<= CHAR_ALPHABET_LOWER_CASE_ENDING))
		{
			printf("Character \'%c\' entered by you, is a CONSONANT CHARACTER from the english alphabet!!!\n\n", ch);
		}
		else if (ch_value >= CHAR_DIGIT_BEGINNING && ch_value <= CHAR_DIGIT_ENDING)
		{
			printf("Character '\%c\' entered by you is a special character !!!\n\n", ch);
		}
		else {
			printf("Character \'%c\' entered by yu is a special character !!!\
			n\n", ch);
		}
		break;
	}
	printf("Switch case block complete!!!\n");

	return(0);
}