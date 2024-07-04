#include <stdio.h>

int main(void) {
	//variable declarations
	int i, j;

	//code
	printf("\n\n");

	printf("Outer loop prints odd numbers between 1 to 10. \n\n");
	printf("Inner loop prints even numbers between 1 to 10 for every odd number printed by outer loop. \n\n");

	for (i = 1; i <= 10; i++) {
		if (i % 2 != 0)//if number is odd...
		{
			printf("i = %d\n", i);
			printf("-------\n");

			for (j = 1; j <= 10; j++) {
				if (j % 2 == 0)//if number is even...
				{
					printf("\tj = %d\n", j);
				}
				else {
					continue;
				}
			}
			printf("\n\n");
		}
		else {
			continue;
		}
	}
	printf("\n\n");
	return(0);

}