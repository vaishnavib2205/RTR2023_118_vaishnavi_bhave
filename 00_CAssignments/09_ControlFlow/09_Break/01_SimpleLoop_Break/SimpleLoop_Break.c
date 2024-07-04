#include<stdio.h>
#include<conio.h>

int main(void) {

//variable declarations
	int i;
	char ch;

	//code
	printf("\n\n");

	printf("Printing Even Numbers from 1 to 100 fro every user input. Exitting the loop when user enters character 'Q' or 'q': \n\n");
	printf("Enter character 'Q' or 'q' to exit the loop: \n\n");

	for (i = 1; i <= 100; i++) {
		printf("\t%d\n", i);
		ch = getch();
		if (ch == 'Q' || ch == 'q') {
			break;
		}
	}

	printf("\n\n");
	printf("Exitting Loop...");
	printf("\n\n");

	return(0);

}