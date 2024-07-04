#include <stdio.h>

int main(void) {
	//variable declarations
	float vb_f;
	float vb_f_num = 1.7f; //simply change this value only to get different outputs...

	//code
	printf("\n\n");

	printf("Printing Numbers %f to %f : \n\n", vb_f_num, (vb_f_num * 10.0f));

	for (vb_f = vb_f_num; vb_f <= (vb_f_num * 10.0f); vb_f = vb_f + vb_f_num)
	{
		printf("\t%f\n", vb_f);

	}
	printf("\n\n");

	return(0);
}