#include <stdio.h>
int main(void) {
	//variable declarations
	int VB_i, VB_j, VB_k;

	//code
	printf("\n\n");

	VB_i = 1;
	while (VB_i <= 10) {
		printf("i = %d\n", VB_i);
		printf("------\n\n");

		VB_j = 1;
		while (VB_j <= 5) {
			printf("\tj = %d\n", VB_j);
			printf("\t------\n\n");

			VB_k = 1;
			while (VB_k <= 3)
			{
				printf("\t\tk = %d\n", VB_k);
				VB_k++;
			}
			printf("\n\n");
			VB_j++;
		}
		printf("\n\n");
		VB_i++;
	}
	return(0);
}