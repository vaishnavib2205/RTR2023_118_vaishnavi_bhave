#include<stdio.h>

//Defining struct

struct MyData {
	int i;
	float f;
	double d;
	char c;
};

struct MyData data = { 5, 9.1f, 3.786623, 'N' };

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
