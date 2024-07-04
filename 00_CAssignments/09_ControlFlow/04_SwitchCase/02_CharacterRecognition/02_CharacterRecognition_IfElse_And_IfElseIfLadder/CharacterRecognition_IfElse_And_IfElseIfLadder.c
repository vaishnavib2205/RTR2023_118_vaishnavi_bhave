
//ASCII Values for 'A' to 'Z' => 65 to 90
#define CHAR_ALPHABET_UPPER_CASE_BEGINNING 65
#define CHAR_ALPHABET_UPPER_CASE_ENDING 90

//ACSII Values For 'a' to 'z' => 97 to 122
#define CHAR_ALPHABET_LOWER_CASE_BEGINNING 97
#define CHAR_ALPHABET_LOWER_CASE_ENDING 122

//ASCII Values for '0' to '9' => 48 to 57
#define CHAR_DIGIT_BEGINNING 48
#define CHAR_DIGIT_ENDING 57

int main(void)
{
	//variable declarations
	char VB_ch;
	int VB_ch_value;

	//code
	printf("\n\n");

	printf("Enter Character :");
	VB_ch = getch();

	printf("\n\n");

	if ((VB_ch == 'A' || (VB_ch == 'a') || (VB_ch == 'E' || (VB_ch == 'e') || (VB_ch == 'I' || VB_ch == 'i') || (VB_ch == 'O' || VB_ch == 'o') || (VB_ch == 'U') || VB_ch == 'u'))
	{
		printf("Character \'%c\' Entered By You, Is A VOWEL CHARACTER From the English Alphabet !!!\n\n", VB_ch);
	}

	else
	{
		VB_ch_value = (int VB_ch);

		//If The CHaracter Has ASCII Value Between 65 AND 90 Between 97 AND 122,
		// It is Still A Letter of the Alphabet, But It is A 'CONSONANT', and
		// NOT a 'VOWEL' ...

		if ((VB_ch_value >= CHAR_ALPHABET_UPPER_CASE_BEGINNING && VB_ch_value <= CHAR_ALPHABET_UPPER_CASE_ENDING) || (VB_ch_value >= CHAR_ALPHABET_LOWER_CASE_BEGINNING && PB_ch_value <= CHAR_ALPHABET_LOWER_CASE_ENDING))
		{
			printf("Character \'%c\' Entered By You, Is A CONSONENT CHARACTER From the English Alphabet !!!\n\n",VB__ch);
		}
		else if (VB_ch_value >= CHAR_DIGIT_BEGINNING && VB_ch_value <= CHAR_DIGIT_ENDING)
		{
			printf("Character \'%c\' Entered By You, Is A DIGIT CHARACTER !!!\n\n",VB_ch);
		}
		else
		{
			printf("Character \'%c\' Entered By You, Is A SPECIAL CHARACTER !!!\n\n",VB_ch);
		}

	}
	return (0);
}
