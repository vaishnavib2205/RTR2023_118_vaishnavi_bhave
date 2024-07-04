#include<stdio.h>
int main(void)
{
	//variable declarations

	int i, j;

	//code

	printf("\n\n");

	i = 1;
	do
	{
		printf("i = %d\n", i);
		printf("_______\n\n");
		j = 1;
		do
		{
			printf("\t j = %d\n", j);
			j++;
		} while (j <= 5);
		i++;
		printf("\n\n");
	} while (i <= 10);
	return(0);
}