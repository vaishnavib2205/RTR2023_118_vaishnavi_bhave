#include <stdio.h>

//defining struct

struct MyPoint {
	int x;
	int y;
};

struct MyPoint point_A, point_B, point_C, point_D, point_E;

int main(void) {

//code
//Assigning data values to he fata members of struct mypoint variable point_a

	point_A.x = 3;
	point_A.y = 0;

	point_B.x = 1;
	point_B.y = 2;

	point_C.x = 9;
	point_C.y = 6;

	point_D.x = 8;
	point_D.y = 2;

	point_E.x = 11;
	point_E.y = 8;

	//Displaying values of the data  members of struct MyPoint all variables

	printf("\n\n");
	printf("Co-rdinates (x, y) of point 'A' are: (%d, %d)\n\n", point_A.x, point_A.y);
	printf("Co-rdinates (x, y) of point 'B' are: (%d, %d)\n\n", point_B.x, point_B.y);
	printf("Co-rdinates (x, y) of point 'C' are: (%d, %d)\n\n", point_C.x, point_C.y);
	printf("Co-rdinates (x, y) of point 'D' are: (%d, %d)\n\n", point_D.x, point_D.y);
	printf("Co-rdinates (x, y) of point 'E' are: (%d, %d)\n\n", point_E.x, point_E.y);

	return(0);
}