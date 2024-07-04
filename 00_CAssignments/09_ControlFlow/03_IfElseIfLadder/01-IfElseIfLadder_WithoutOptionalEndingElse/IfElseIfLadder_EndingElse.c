#include<stdio.h>

int main(void)
{
	//Varible declarations
	int VB_num;

	//code
	printf("\n\n");

	printf("Enter value for 'num' :");
	scanf("%d ", &VB_num);

	// IF - ELSE - IF LADDER BEGINS FROM HERE....

	if (VB_num < 0)
	{
		printf("Num = %d is less than O(NEGATIVE) !!!\n\n", VB_num);
	}
	else if ((VB_num > 0) && (VB_num <= 100))
	{
		printf("Num = %d is Between 0 And 100 !!!\n\n", VB_num);
	}
	else if ((VB_num > 100) && (VB_num <= 200))
	{
		printf("Num = %d is Between 100 And 200 !!!\n\n", VB_num);
	}
	else if ((VB_num > 200) && (VB_num <= 300))
	{
		printf("Num = %d is Between 200 And 300 !!!\n\n", VB_num);
	}
	else if ((VB_num > 300) && (VB_num <= 400))
	{
		printf("Num = %d is Between 300 And 400 !!!\n\n",VB_num);
	}
	else if ((VB_num > 400) && (VB_num <= 500))
	{
		printf("Num = %d is Between 400 And 500 !!!\n\n", VB_num);
	}
	else
	{
		printf("Num = %d Is Greater Than 500 !!!\n\n", VB_num);
	}

	return(0);

}

