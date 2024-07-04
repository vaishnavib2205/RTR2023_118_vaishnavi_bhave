#include <stdio.h>

int main(void) {

	int VB_i_num, VB_num, VB_i;

	//code
	printf("\n\n");

	printf("Enter an integer value from which iteration must begin: ");
	scanf("%d", &VB_i_num);

	printf("How many digits do you want to print from %d onwards?: ", VB_i_num);
	scanf("%d", &VB_num);

	printf("Printing digits %d to %d : \n\n", VB_i_num, (VB_i_num + VB_num));

	for (VB_i = VB_i_num; VB_i <= (VB_i_num + VB_num); VB_i++) {
		printf("\t%d\n", VB_i);
	}
	printf("\n\n");

	return(0);
}