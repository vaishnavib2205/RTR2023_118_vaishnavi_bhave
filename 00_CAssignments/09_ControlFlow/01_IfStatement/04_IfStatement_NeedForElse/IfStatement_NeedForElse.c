#include<stdio.h>

int main(void)
{
	int VB_age;

	printf("\n\n");

	printf("Enter Age : ");
	scanf("%d", &VB_age);

	if (VB_age >= 18)
	{
		printf("You are eligible for Voting !!!\n\n");
	}
	else
	printf("You are NOT eligible for Voting !!!\n\n");

	return(0);

}
