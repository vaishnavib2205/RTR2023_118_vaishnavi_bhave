#include<stdio.h>

int main(void)
{
	//variable declarations
	char option, ch = '\0';

	//code
	printf("\n\n");
	printf("Once the infinite loop begins, enter 'Q' or 'q' to quit the infinite for loop: \n\n");
	printf("Enter 'Y' or 'y' to initiate user controlled infinite loop: ");
	printf("\n\n");
	option = getch();

	if (option == 'Y' || option == 'y') {
		while (1) //Infinite Loop
		{
			printf("In Loop...\n");
			ch = getch();
			if (ch == 'Q' || ch == 'q')
				break;//User controlled exitting from infinite loop
		}
		printf("\n\n");
		printf("Exitting user controlled infinite loop...");
		printf("\n\n");
	}
	else {
		printf("You must press 'Y' or 'y' to initiate the user controlled infinite loop....Please try again....\n\n");

		return(0);
	}
}