#include<stdio.h>

// DEFINING STRUCT ...

struct MyPoint
{
	int x;
	int y;
};

int main(void)
{
	// variable declaration 
	struct MyPoint point_A, point_B, point_C, point_D, point_E; // Declaring a 5 struct variables type 'struct MyPoint' 

	// code
	// user input for the data members of 'struct MyPoint' variable 'point_A'

	printf("\n\n");
	printf("Enter X-Coordinate For A Point : ");
	scanf("%d", &point_A.x);
	printf("Enter Y - Coordinate For A Point : ");
	scanf("%d", &point_A.y);

	// user input for the data members of 'struct MyPoint' variable 'point_B'

	printf("\n\n");
	printf("Enter X-Coordinate For B Point : ");
	scanf("%d", &point_B.x);
	printf("Enter Y - Coordinate For B Point : ");
	scanf("%d", &point_B.y);

	// user input for the data members of 'struct MyPoint' variable 'point_C'

	printf("\n\n");
	printf("Enter X-Coordinate For C Point : ");
	scanf("%d", &point_C.x);
	printf("Enter Y - Coordinate For C Point : ");
	scanf("%d", &point_C.y);

	// user input for the data members of 'struct MyPoint' variable 'point_D'

	printf("\n\n");
	printf("Enter X-Coordinate For D Point : ");
	scanf("%d", &point_D.x);
	printf("Enter Y - Coordinate For D Point : ");
	scanf("%d", &point_D.y);

	// user input for the data members of 'struct MyPoint' variable 'point_E'

	printf("\n\n");
	printf("Enter X-Coordinate For E Point : ");
	scanf("%d", &point_E.x);
	printf("Enter Y - Coordinate For E Point : ");
	scanf("%d", &point_E.y);

	// Displaying values Of The Data Members Of 'struct MyPoint (All Variables)

	printf("\n\n");

	printf("Co-ordinates (x, y) Of Point 'A' Are : (%d , %d)\n\n", point_A.x, point_A.y);
	printf("Co-ordinates (x, y) Of Point 'B' Are : (%d , %d)\n\n", point_B.x, point_B.y);
	printf("Co-ordinates (x, y) Of Point 'C' Are : (%d , %d)\n\n", point_C.x, point_C.y);
	printf("Co-ordinates (x, y) Of Point 'D' Are : (%d , %d)\n\n", point_D.x, point_D.y);
	printf("Co-ordinates (x, y) Of Point 'E' Are : (%d , %d)\n\n", point_E.x, point_E.y);

	return 0;

}