#include <stdio.h>

//defining struct
struct MyData {
	int i;
	float f;
	double d;
};

struct MyData data;

int main(void) {
	//variable declarations
	int i_size;
	int f_size;
	int d_size;
	int struct_MyData_size;

	//code
	//Assigning data values to the data valyes to the data members of struct MyData
	data.i = 30;
	data.f = 11.45f;
	data.d = 1.2995;

	//Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("Data members of 'struct MyData' are: \n\n");
	printf("i = %d\n", data.i);
	printf("f = %f\n", data.f);
	printf("d = %lf\n", data.d);

	//calculating sizes in bytes of the data members of struct mydata
	i_size = sizeof(data.i);
	f_size = sizeof(data.f);
	d_size = sizeof(data.d);

	printf("\n\n");
	printf("Sizes in bytes of data members of struct mydata are: \n\n");
	printf("Size of 'i' = %d bytes\n", i_size);
	printf("Size of 'f' = %d bytes\n", f_size);
	printf("Size of 'd' = %d bytes\n", d_size);

	//Calculating size in bytes of the entire struct mydata
	struct_MyData_size = sizeof(struct MyData);

	printf("\n\n");
	printf("Size of struct MyData : %d bytes\n\n", struct_MyData_size);
	return(0);
}