#include <stdio.h>
int main(void) {

//variable declarations
	int VB_i, VB_j;

	//cde
	printf("\n\n");

	for (VB_i = 1; VB_i <= 10; VB_i++) {
		printf("VB_i = %d\n", VB_i);

		printf("_____\n\n");

		for (VB_j = 1; VB_j <= 5; VB_j++) {
			printf("\tj = %d\n", VB_j);
		}
		printf("\n\n");
	}
	return(0);
}