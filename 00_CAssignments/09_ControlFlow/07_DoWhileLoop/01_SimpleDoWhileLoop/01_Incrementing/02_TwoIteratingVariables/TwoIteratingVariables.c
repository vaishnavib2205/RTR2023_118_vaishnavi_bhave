#include <stdio.h>
int main(void) {
	//variable declarations
	int VB_i, VB_j;

	//code
	printf("\n\n");

	printf("Printing digits 1 to 10 and 10 to 100: \n\n");

	VB_i = 1;
	VB_j = 10;

	do{
		printf("\t %d \t %d\n", VB_i, VB_j);
		VB_i++;
		
		VB_j = VB_j + 10;
	}while(VB_i <= 10, VB_j <= 100);

	printf("\n\n");
	return(0);
}