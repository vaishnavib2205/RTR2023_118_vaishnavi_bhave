#include <stdio.h>
int main(void) {
	//variable declarations

	int i, j;

	//code
	printf("\n\n");

	printf("Printing digits 1 to 10 and 10 t 100: \n\n");

	i = 1;
	j = 10;

	while (i <= 10, j <= 100) {
		printf("\t %d \t %d\n", i, j);
		i++;
		j = j + 10;
	}
	printf("\n\n");

	return(0);
}