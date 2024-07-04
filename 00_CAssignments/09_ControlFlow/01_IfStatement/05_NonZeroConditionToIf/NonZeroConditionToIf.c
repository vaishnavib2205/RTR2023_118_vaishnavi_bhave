#include<stdio.h>

int main(void)
{
	int VB_a;

	printf("\n\n");

	VB_a = 5;
	if (VB_a)
	{
		printf("if-block 1 : 'A' Exists And Has Value = %d !!!\n\n", VB_a);
	}
	VB_a = -5;
	if (VB_a)
	{
		printf("if-block 2 : 'A' Exists And Has Value = %d !!!\n\n", VB_a);
	}

	VB_a = 0;
	if (VB_a)
	{
		printf("if-block 3 : 'A' Exists And Has Value = %d !!!\n\n", VB_a);
	}

	printf("All Three if_statement Are Done !!!\n\n");

	return(0);


}