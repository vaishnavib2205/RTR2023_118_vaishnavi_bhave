#include<stdio.h>
int main(void){

//variable declarations
	int VB_i, VB_j;

	//code
	printf("\n\n");

	printf("Printing digits 10 to 1 and 100 to 10: \n\n");

	VB_i = 10;
	VB_j = 100;

	do {
		printf("\t %d \t %d\n", VB_i, VB_j);
		VB_i--;
		VB_j = VB_j - 10;
	} while (VB_i >= 1, VB_j >= 10);

	printf("\n\n");

	return(0);

}
