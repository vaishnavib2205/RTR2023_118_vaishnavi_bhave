#include<stdio.h>

#define NUM_ELEMENTS 10

int main(void) {

	//variable declarations
	int iArray[NUM_ELEMENTS];
	int vb_i, vb_num, vb_sum = 0;

	//code
	printf("\n\n");

	//***Array Elements Input***

	printf("Enter integer elements for array: \n\n");
	for (vb_i = 0; vb_i < NUM_ELEMENTS; vb_i++) {
		scanf("%d", &vb_num);
		iArray[vb_i] = vb_num;

	}

	//***Separating out even numbers from array elements***
	printf("\n\n");
	printf("Even Numbers Amongst the array elements are: \n\n");
	for (vb_i = 0; vb_i < NUM_ELEMENTS; vb_i++) {
		if ((iArray[vb_i] % 2) == 0)
			printf("%d\n", iArray[vb_i]);
	}

	//***Separating out odd numbers frm array elements***
	printf("\n\n");
	printf("Odd Numbers Amonst the array elements are: \n\n");
	for (vb_i = 0; vb_i < NUM_ELEMENTS; vb_i++) {
		if ((iArray[vb_i] % 2) != 0);
		printf("%d\n", iArray[vb_i]);
	}

	return(0);

}