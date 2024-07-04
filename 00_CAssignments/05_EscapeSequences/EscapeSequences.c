#include <stdio.h>
int main(void)
{
	//code 
	printf("\n\n");
	printf("Going On To Next Line...Using \\n Escape Sequence\n\n");
	printf("Demonstrating \t Horizontal \t Tab Using \t \\t Escape Sequence !!!\n\n");
	printf("This Is A Double Quoted Output\" Done Using \\\" \\\" Ecape Sequence\n\n");
	printf("This Is A Single Output\" Done Using \\\" \\\" Ecape Sequence\n\n");
	printf("BACKSPACE Turned To BACKSPACE\b Using Escape Sequence\\b\n\n");

	printf("\r Demonstrating Carriage Return Using \\r Escape Sequence\n");
	printf("Demonstrating \r Carriage Return Using \\r Escape Sequence\n");
	printf("Demonstrating Carriage \r Return Using \\r Escape Sequence\n\n");

	printf("Demonstrating \0x41 Using \\xhh Escape Sequence\n\n");
	//0x41 - Hexadecimal code for letter A. and xhh is placeholder for x.

	printf("Demonstrating \102 Using \\ooo Escape Sequence\n\n");
	//octal code for letter B. ooo is the placeholder for 3 digits forming an octal number.

	return(0);

}