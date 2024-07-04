#include<stdio.h>//for ptintf()
#include<conio.h>//for getch()

int main(void)
{
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
	printf("'A' or 'a' for Addition : \n");
	printf("'S' or 's' for Subtraction : \n");
	printf("'M' or 'm' for Multiplication : \n");
	printf("'D' or 'd' for Division : \n");

	printf("Enter Option: ");
	option = getch();

	printf("\n\n");

	if (option == 'A' || option == 'a') {
		result = a + b;
		printf("Addition of A =%d and B=%d gives result%d!!!", a, b, result);
	}
	else if(option == 'S' || option == 's') {
		if (a >= b) {
			result = a - b;
			printf("Subtraction of B =%d from A=%d gives result %d!!!\n\n", b, a, result);
		}
		else {
			result = b - a;
			printf("Subtraction of A =%d from B =%d gives result %d!!!\n\n", a, b, result);
		}
	}
	else if(option == 'M' || option == 'm') {
		result = a * b;
		printf("Multiplication of A = %d and B = %d gives result %d!!!\n\n", a, b, result);
	}
	else if (option == 'D' || option == 'd') {
		printf("Enter option in character: \n\n");
		printf("'Q' or 'q' or '%%' for remainder upon division: \n");

		printf("Enter Option: ");
		option_division = getch();

		printf("\n\n");

		if (option_division == 'Q' || option_division == 'q' || option_division == '/')
		{
			if (a == b)
			{

			}
		}
	}

}