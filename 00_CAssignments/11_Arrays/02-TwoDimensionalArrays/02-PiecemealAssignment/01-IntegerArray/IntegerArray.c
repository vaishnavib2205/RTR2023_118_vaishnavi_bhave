#include <stdio.h>
int main(void) {
//variable declarations

	int iArray[3][5];
	int int_size;
	int iArray_size;
	int iArray_num_elements, iArray_num_rows, iArray_num_columns;
	int i, j;


	//code
	printf("\n\n");

	int_size = sizeof(int);

	iArray_size = sizeof(int);

	iArray_size = sizeof(iArray);
	printf("Size of two dimensional (2D) integer array is= %d\n\n", iArray_num_rows);

	iArray_num_columns = sizeof(iArray[0]) / int_size;
	printf("Number of rows in two dimensional (2D) array is = %d\n\n", iArray_num_columns);

	iArray_num_elements = iArray_num_rows * iArray_num_columns;
	printf("Number of elements in two dimensional (2D) array is = %d\n\n", iArray_num_elements);

	printf("\n\n");
	printf("Elements in the 2D array: \n\n");

	//******************************PIECE MEAL ASSIGNMENT****************************************
	//******************* ROW 1 *******************

	iArray[0][0] = 21;
	iArray[0][1] = 42;
	iArray[0][2] = 63;
	iArray[0][3] = 84;
	iArray[0][4] = 105;

	//****************** ROW 2 *********************
	iArray[1][0] = 22;
	iArray[1][1] = 44;
	iArray[1][2] = 66;
	iArray[1][3] = 88;
	iArray[1][4] = 110;

	//****************** ROW 3 **********************
	iArray[2][0] = 23;
	iArray[2][1] = 46;
	iArray[2][2] = 69;
	iArray[2][3] = 92;
	iArray[2][4] = 115;

	// **************************** DISPLAY *****************************

	for (i = 0; i < iArray_num_rows; i++) {
		printf("******************************* ROW %d ****************************\n", (i + 1));

		for (j = 0; j < iArray_num_columns; j++) {
			printf("iArray[%d][%d] = %d\n", i, j, iArray[i][j]);
		}
		printf("\n\n");
	}
	return(0);
}