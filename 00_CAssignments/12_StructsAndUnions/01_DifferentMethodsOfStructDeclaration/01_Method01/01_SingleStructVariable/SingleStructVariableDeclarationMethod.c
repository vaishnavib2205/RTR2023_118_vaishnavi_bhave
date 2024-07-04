#include<stdio.h>

//defining struct
struct MyData
{
	int vb_i;
	float vb_f;
	double vb_d;
	char vb_c;
} data; //declaring a single struct variable f type 'struct mydata' globally...

int main(void) {

	//variable declarations
	int i_size;
	int f_size;
	int d_size;
	int struct_MyData_size;

	//code
	//Assigning Data values to the data members of 'Struct MyData'

	data.vb_i = 30;
	data.vb_f = 11.45f;
	data.vb_d = 1.2995;

	//displaying values of the data members f struct mydata 

	i_size = sizeof(data.vb_i);
	f_size = sizeof(data.vb_f);
	d_size = sizeof(data.vb_d);

	//displaying values of the data members of struct mydata
	printf("\n\n");
	printf("SIZES (in bytes) of data members of struct mydata are: \n\n");
	printf("Size of 'i' = %d\n", i_size);
	printf("Size of 'f' = %d\n", f_size);
	printf("Size of 'd' = %d\n", d_size);

	//Calculating size (in bytes) of the entire 'struct MyData'
	struct_MyData_size = sizeof(struct MyData); //can also give struct name -> sizeof(MyData)

	printf("\n\n");
	printf("Size of 'struct MyData' : %d bytes\n\n", struct_MyData_size);
	return(0);

}



