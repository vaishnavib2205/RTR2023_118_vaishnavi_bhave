#include <stdio.h>

//defining struct
struct MyData {
	int i;
	float f;
	double d;
	char c;
} data = {30, 4.4f, 11.451995, 'A'};

int main(void) {
	//Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("Data members of 'struct MyData' are: \n\n");
	printf("i = %d\n", data.i);
	printf("f = %f\n", data.f);
	printf("d = %lf\n", data.d);
	printf("c = %c\n", data.c);
	
	return(0);
}
