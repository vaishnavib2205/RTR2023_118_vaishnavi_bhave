#include <stdio.h> //for printf()
#include <conio.h> //for getch()

int main(void) {
	//variable declarations 
	int a, b;
	int result;

	char option, option_division;
	
	//code
	printf("\n\n");

	printf("Enter value for 'A': ");
	scanf("%d", &a);

	printf("Enter value for 'B': ");
	scanf("%d", &b);

	printf("Enter option in character: \n\n");
	printf("'A' or 'a' for addition : \n\n");
	printf("'S' or 's' for subtraction : \n\n");
	printf("'M' or 'm' for multiplication : \n\n");
	printf("'D' or 'd' for division: \n\n");

	printf("Enter Option: ");
	option = getch();

	printf("\n\n");

	switch (option) {
		//fall through condition for 'A' and 'a'
	case 'A':
	case 'a': 
		result = a + b;
		printf("Addition of A =%d and B =%d gives result %d!!!\n\n", a, b, result);
		break;

		//fall through condition for 'S' and 's'
	case 'S':
	case 's':
		if (a >= b) {
			result = a - b;
			printf("Subtraction of B = %d from A = %d gives result %d!!!\n\n", a, b, result);

	}
		else {
			result = b - a;
			printf("Subtraction of A = %d from B = %d gives result %d!!!\n\n", a, b, result);

		}
		break;

		//fall through condition for 'D' and 'd'
	case 'D':
	case 'd':
		printf("Enter option in character : \n\n");
		printf("'Q' or 'q' or '/' for quotient upon division: \n");
		printf("'R' or 'r' or '%%' for remainder upon division: \n");

		printf("Enter option: ");
		option_division = getch();

		printf("\n\n");

		switch (option_division) {
			//fall through condition 'Q' and 'q' and '/'
		case 'Q':
		case 'q':
		case '/':
			if (a >= b) {
				result = a / b;
				printf("Division of A = %d by B = %d gives quotient = %d!!!\n\n", a, b, result);
			}
			else {
				result = b / a;
				printf("Division of B =%d by A = %d gives quotient = %d!!!\n\n", b, a, result);
			}
			break; //break of case 'Q' or case 'q' or case '/'
			//fall through condition for 'R' or case 'r' and '%'
		case 'R':
		case 'r':
		case '%':
			if (a >= b) {
				result = a % b;
				printf("Division of A= 5d By B=%d gives remainder = %d!!!\n\n", a, b, result);
			}
			else {
				result = b % a;
				printf("Division of B=%d by A = %d gives remainder =%d !!!\n\n", b, a, result);
			}break; //break of case 'R' or case 'r' or case '%'

		default: //default case for switch(option)
			printf("Invalid character %c entered!!! Please try again...\n\n", option);
			break;
		}//ending curly braces of switch(option)

		printf("Switch case block complete!!!\n");

		return(0);
	}

}