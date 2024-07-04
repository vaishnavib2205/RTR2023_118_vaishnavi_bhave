#include<stdio.h>


int main(void)
{
	// DEFINING STRUCT ...

	struct MyPoint
	{
		int x;
		int y;
	}point_A, point_B, point_C, point_D, point_E; // Declaring 5 struct variables type 'struct MyPoint' Locally...



	//code
	// Assigning data values to the data members of 'Struct MyPoint' variable 'point_A'

	point_A.x = 3;
	point_A.y = 0;

	// Assigning data values to the data members of 'Struct MyPoint' variable 'point_B'

	point_B.x = 1;
	point_B.y = 2;

	// Assigning data values to the data members of 'Struct MyPoint' variable 'point_C'

	point_C.x = 9;
	point_C.y = 0;

	// Assigning data values to the data members of 'Struct MyPoint' variable 'point_D'

	point_D.x = 8;
	point_D.y = 2;

	// Assigning data values to the data members of 'Struct MyPoint' variable 'point_E'

	point_E.x = 11;
	point_E.y = 8;

	// Displaying Sizes (In Bytes) Of The Data Members Of 'struct MyPoint (All Variables)

	printf("\n\n");

	printf("Co-ordinates (x, y) Of Point 'A' Are : (%d , %d)\n\n", point_A.x, point_A.y);
	printf("Co-ordinates (x, y) Of Point 'B' Are : (%d , %d)\n\n", point_B.x, point_B.y);
	printf("Co-ordinates (x, y) Of Point 'C' Are : (%d , %d)\n\n", point_C.x, point_C.y);
	printf("Co-ordinates (x, y) Of Point 'D' Are : (%d , %d)\n\n", point_D.x, point_D.y);
	printf("Co-ordinates (x, y) Of Point 'E' Are : (%d , %d)\n\n", point_E.x, point_E.y);

	return 0;

}