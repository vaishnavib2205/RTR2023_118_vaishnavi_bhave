#include<stdio.h>

int main(void)
{
	// Defining Struct
	struct MyData
	{
		int i;
		float f;
		double d;
		char c;
	}data = { 5,9.1f,3.78623, 'N' }; // Inline Initialization of struct variable 'data' of type 'struct MyData'


	//code
	//Displaying Values Of The Data  Members Of 'struct MyData'

	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("i = %d\n", data.i);
	printf("f = %f\n", data.f);
	printf("d = %lf\n", data.d);
	printf("c = %c\n", data.c);

	return 0;
}