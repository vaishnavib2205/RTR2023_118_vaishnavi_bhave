#include <stdio.h>

int main(void)
{
	// he variable declarations aaheeeeeeeee
	int iArray[5][3] = { {1, 2, 3}, {2, 4, 6}, {3, 6, 9}, {4, 8, 12}, {5,10, 15} }; //innline initialization
	int int_size;
	int iArray_size;
	int iArray_num_elements, iArrays_num_rows, iArrays_num_columns;

	//code
	printf("\n\n");
	int_size = sizeof(int);

	iArray_size = sizeof(iArray);
	printf("Size of two dimensional (2D) integer array is = %d\n\n", iArray_size);

	iArrays_num_rows = iArray_size / sizeof(iArray[0]);
	printf("Number of Rows in two Dimensional (2D) integer array is= %d\n\n", iArrays_num_columns);

	iArrays_num_columns = sizeof(iArray[0]) / int_size;
	printf("Number of columns in two dimensional (2D) integer array is = %d\n\n", iArrays_num_columns);

	iArray_num_elements = iArrays_num_rows * iArrays_num_columns;
	printf("Number of columns in two dimensional (2D) integer array is = %d\n\n", iArray_num_elements);

	printf("\n\n");
	printf("Elements in the 2D array: \n\n");

	// Array indices begin from 0, hence 1st rw is actually 0th row and 1st column actually 0th column

	//row 1
	printf("*************************** ROW 1 ********************************\n");
	printf("iArray[0][0] = %d\n", iArray[0][0]);//col1 ... 0th
	printf("iArray[0][1] = %d\n", iArray[0][1]);//col2 ... 1st
	printf("iArray[0][2] = %d\n", iArray[0][2]);//col3 ... 2nd

	//row2
	printf("iArray[1][0] = %d\n", iArray[1][0]);
	printf("iArray[1][1] = %d\n", iArray[1][1]);
	printf("iArray[1][2] = %d\n", iArray[1][2]);

	//row3
	printf("iArray[2][0] = %d\n", iArray[2][0]);
	printf("iArray[2][1] = %d\n", iArray[2][1]);
	printf("iArray[2][2] = %d\n", iArray[2][2]);

	//row4
	printf("iArray[3][0] = %d\n", iArray[3][0]);
	printf("iArray[3][1] = %d\n", iArray[3][1]);
	printf("iArray[3][2] = %d\n", iArray[3][2]);

	//row5
	printf("iArray[4][0] = %d\n", iArray[4][0]);
	printf("iArray[4][1] = %d\n", iArray[4][1]);
	printf("iArray[4][2] = %d\n", iArray[4][2]);

	printf("\n\n");

	return(0);


}