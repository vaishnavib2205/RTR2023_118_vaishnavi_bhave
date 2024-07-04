#include<stdio.h>
int main(void)
{
	//variable declarations
	char option, ch = '\0';

	//code

	printf("\n\n");

	printf("Once The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The infinite For Loop : \n\n");

	printf("Enter 'Y' or 'y' to initiate user controlled infinite loop : ");

	printf("\n\n");

	option = getch();

	if (option == 'Y' || option == 'y')
	{
		do
		{
			printf("In Loop...\n");
			ch = getch();
			if (ch == 'Q' || ch == 'q')
			{
				break;//User Controlled Exitting From infinite loop
			}
		} while (1);
		printf("\n\n");
		printf("EXITTING USER CONTROLLED INFINITE LOOP....");
		printf("\n\n");

	}
	else
	{
		printf("You Must Press 'Y' or 'y' To Initialize The user controlled infinite loop .... plese try again .... \n\n");

		return(0);
	}

}