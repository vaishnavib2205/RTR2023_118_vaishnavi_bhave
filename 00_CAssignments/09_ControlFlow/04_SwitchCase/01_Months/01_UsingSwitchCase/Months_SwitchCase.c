#include <stdio.h>
int main(void)
{
	//variable decarations kela
	int num_month;

	//code suruuuuu ^.^
	printf("\n\n");

	printf("Enter number of month(1 to 12) : ");
	scanf("%d", &num_month);

	printf("\n\n");

	switch (num_month)
	{
	case 1: //if statement sarkha asta 
		printf("Month Number %d is January!!!\n\n", num_month);
		break;

	case 2: //else if statement sarkha asta 
		printf("Month Number %d is February!!!\n\n", num_month);
		break;

	case 3: //else if statement sarkha asta 
		printf("Month Number %d is March!!!\n\n", num_month);
		break;

	case 4: //else if statement sarkha asta 
		printf("Month Number %d is April!!!\n\n", num_month);
		break;

	case 5: //else if statement sarkha asta 
		printf("Month Number %d is May!!!\n\n", num_month);
		break;

	case 6: //else if statement sarkha asta 
		printf("Month Number %d is June!!!\n\n", num_month);
		break;

	case 7: //else if statement sarkha asta 
		printf("Month Number %d is July!!!\n\n", num_month);
		break;

	case 8: //else if statement sarkha asta 
		printf("Month Number %d is August!!!\n\n", num_month);
		break;

	case 9: //else if statement sarkha asta 
		printf("Month Number %d is September!!!\n\n", num_month);
		break;

	case 10: //else if statement sarkha asta 
		printf("Month Number %d is October!!!\n\n", num_month);
		break;

	case 11: //else if statement sarkha asta 
		printf("Month Number %d is November!!!\n\n", num_month);
		break;

	case 12: //else if statement sarkha asta 
		printf("Month Number %d is December!!!\n\n", num_month);
		break;

	default: // ending else statement sarkha
		printf("Inavlid month number %d entered!!! Please try again...\n\n", num_month);
		break;

		return(0);
		//0 - success sathi :)
	//samplaaa program ;)
	}
}