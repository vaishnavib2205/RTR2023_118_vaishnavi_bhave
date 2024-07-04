#include <stdio.h>
int main(void) {
	
	//variable declarations
	float VB_f;
	float VB_f_num = 1.7f; //simply channge this value only to get different outputs...

	//code
	printf("\n\n");

	printf("Printing numbers %f to %f: \n\n", VB_f_num, (VB_f_num * 10.0f));

	VB_f = VB_f_num;
	while (VB_f <= (VB_f_num * 10.0f))
	{
		printf("\t%f\n", VB_f);
		VB_f = VB_f + VB_f_num;

	}
	printf("\n\n");

	return(0);

}