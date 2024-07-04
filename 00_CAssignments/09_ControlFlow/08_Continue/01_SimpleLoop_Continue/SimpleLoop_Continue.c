#include<stdio.h>
int main(void)
{
	//variable declaration
	int i;

	//code

	printf("\n\n");

	printf("Printing even numbers from 0 to 100 : \n\n");

	for (i = 0; i <= 100; i++)
	{
		//condition for an number to br numn=ber => dividion of number by 2
		// leaves no, remainder(=0)
		//if remainder is not 0, the number is odd ...

		if (i % 2 != 0)
		{
			continue;
		}
		else
		{
			printf("\t%d\t", i);
		}


	}
	printf("\n\n");
	return(0);
}