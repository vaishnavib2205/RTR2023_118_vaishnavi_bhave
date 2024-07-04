#include<stdio.h>

int main(void)
{
	//Varible declarations
	int PB_num;

	//code
	printf("\n\n");

	printf("Enter value for 'num' :");
	scanf("%d ", &PB_num);

	// IF - ELSE - IF LADDER BEGINS FROM HERE....

	if (PB_num < 0)
	{
		printf("Num = %d is less than O(NEGATIVE) !!!\n\n", PB_num);
	}
	else if ((PB_num > 0) && (PB_num <= 100))
	{
		printf("Num = %d is Between 0 And 100 !!!\n\n", PB_num);
	}
	else if ((PB_num > 100) && (PB_num <= 200))
	{
		printf("Num = %d is Between 100 And 200 !!!\n\n", PB_num);
	}
	else if ((PB_num > 200) && (PB_num <= 300))
	{
		printf("Num = %d is Between 200 And 300 !!!\n\n", PB_num);
	}
	else if ((PB_num > 300) && (PB_num <= 400))
	{
		printf("Num = %d is Between 300 And 400 !!!\n\n", PB_num);
	}
	else if ((PB_num > 400) && (PB_num <= 500))
	{
		printf("Num = %d is Between 400 And 500 !!!\n\n", PB_num);
	}
	else if(PB_num > 500)
	{
		printf("Num = %d Is Greater Than 500 !!!\n\n", PB_num);
	}
	else
	{
		printf("Invalid Value Entered !!!\n\n");
	}

	return(0);

}