#include <stdio.h>
int main(void) {

//variable declaration
	int VB_i, VB_j, VB_c;

	//code
	printf("\n\n");

	VB_i = 0;
	while (VB_i < 64) {
		VB_j = 0;
		while (VB_j < 64) {
			VB_c = ((VB_i & 0x8) == 0) ^ ((VB_j & 0x8) == 0);

			if (VB_c == 0)
				printf(" ");

			if (VB_c == 1)
				printf("* ");

			VB_j++;
		}
		printf("\n\n");
		VB_i++;
	}

	return(0);
}