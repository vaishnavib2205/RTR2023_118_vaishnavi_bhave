#include<stdio.h>

int main(void)
{
	// variable declaration
	int iArray_One[5];
	int iArray_Two[5][3];
	int iArray_Three[5][3][2];

	int num_rows_2D;
	int num_columns_2D;

	int num_rows_3D;
	int num_columns_3D;
	int depth_3D;

	// code

	printf("\n\n");
	printf("Size of 1-D integer array iArray_One = %zu\n", sizeof(iArray_One));
	printf("Number Of Elements in 1D integer Array iArray_one = %zu\n", (sizeof(iArray_One) / sizeof(int)));

	printf("\n\n");
	printf("Size of 2-D integer array iArray_One = %zu\n", sizeof(iArray_Two));

	printf("Number Of Rows in 2D integer Array iArray_Two = %zu\n", (sizeof(iArray_Two) / sizeof(iArray_Two[0])));
	num_rows_2D = sizeof(iArray_Two) / sizeof(iArray_Two[0]);

	printf("Number Of Columns in each row in 2D integer Array iArray_Two = %zu\n", (sizeof(iArray_Two[0]) / sizeof(iArray_Two[0][0])));
	num_columns_2D = sizeof(iArray_Two[0]) / sizeof(iArray_Two[0][0]);

	printf("Number Of Elements in 2D integer Array iArray_Two = %zu\n", (num_rows_2D * num_columns_2D));

	printf("\n\n");

	printf("\n\n");

	printf("Size of 3-D integer array iArray_Three = %zu\n", sizeof(iArray_Three));

	printf("Number Of Rows in 3D integer Array iArray_Three = %zu\n", (sizeof(iArray_Three) / sizeof(iArray_Three[0])));
	num_rows_3D = sizeof(iArray_Three) / sizeof(iArray_Three[0]);

	printf("Number Of Columns in each row in 3D integer Array iArray_Three = %zu\n", (sizeof(iArray_Three[0]) / sizeof(iArray_Three[0][0])));
	num_columns_3D = sizeof(iArray_Three[0]) / sizeof(iArray_Three[0][0]);

	printf("Number Of depth in one Column in each row in 3D integer Array iArray_Three = %zu\n", (sizeof(iArray_Three[0][0]) / sizeof(iArray_Three[0][0][0])));
	depth_3D = sizeof(iArray_Three[0][0]) / sizeof(iArray_Three[0][0][0]);

	printf("Number Of Elements in 3D integer Array iArray_Three = %zu\n", (num_rows_3D * num_columns_3D * depth_3D));

	printf("\n\n");

	return 0;

}