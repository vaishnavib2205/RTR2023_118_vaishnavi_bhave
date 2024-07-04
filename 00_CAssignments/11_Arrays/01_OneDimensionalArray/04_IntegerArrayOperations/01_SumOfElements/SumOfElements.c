#include<stdio.h>

#define NUM_ELEMENTS 10

int main(void) {

	//variable declarations
	int iArray[NUM_ELEMENTS];
	int vb_i, vb_num, vb_sum = 0;

	//code
	printf("\n\n");
	printf("Enter Integer Elements For Array: \n\n");
	for (vb_i = 0; vb_i < NUM_ELEMENTS; vb_i++) {
		scanf("%d", &vb_num);
		iArray[vb_i] = vb_num;
	}
	for (vb_i = 0; vb_i < NUM_ELEMENTS; vb_i++) {
		vb_sum = vb_sum + iArray[vb_i];
	}
	printf("\n\n");
	printf("Sum of all Elements of Array = %d\n\n", vb_sum);

	return(0);
}