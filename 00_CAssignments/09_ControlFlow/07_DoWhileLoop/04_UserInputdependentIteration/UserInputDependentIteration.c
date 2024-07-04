#include<stdio.h>

int main(void)
{
	//variable declaractions

	int i_num, num, i;

	//code

	printf("\n\n");

	printf("Enter An Integer value from which iteration must begin : ");
	scanf("%d", &i_num);

	printf("How Many Digits do you want to print from %d Onwards ? : ", i_num);
	scanf("%d", &num);

	i = i_num;

	do
	{
		printf("\t%d\n", i);
		i++;
	}while (i <= (i_num + num));

	printf("\n\n");

	return(0);

}
