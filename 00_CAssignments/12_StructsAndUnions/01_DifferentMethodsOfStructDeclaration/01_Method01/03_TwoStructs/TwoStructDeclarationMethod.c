#include<stdio.h>

// DEFINING STRUCT 

struct MyPoint
{
	int x;
	int y;
}point; // declaring a single varaible of type 'struct MyPoint' globally...

// DEFINING STRUCT 

struct MyPointProperties
{
	int quadrant;
	char axis_location[10];
}point_properties; // declaring a single varaible of type 'struct MyPointProperties' globally...

int main(void)
{
	// code
	// user input for the data members of 'struct MyPoint' variable 'point_A'

	printf("\n\n");
	printf("Enter X-Coordinate For A Point : ");
	scanf("%d", &point.x);
	printf("Enter Y - Coordinate For A Point : ");
	scanf("%d", &point.y);

	printf("\n\n");
	printf("Point Co-ordinates (x,y) Are : (%d,%d) !!!\n\n", point.x, point.y);

	if (point.x == 0 && point.y == 0)
	{
		printf("The Point Is The Origin (%d, %d) !!!\n\n", point.x, point.y);
	}
	else // AtLeast One Of the two values is a non-zero value 
	{
		if (point.x == 0)
		{
			if (point.y < 0)
				strcpy(point_properties.axis_location, "Negative Y");
			if (point.y > 0)
				strcpy(point_properties.axis_location, "Positive Y");
			point_properties.quadrant = 0; // A point lying on Any Of the Co-ordinate axis is not a part of any quadrant...
			printf("The Point Lies On The %s Axis !!!\n\n", point_properties.axis_location);
		}
		else if (point.y == 0) // If 'Y' is zero ..... obviously 'X' Is The NON-ZERO VALUE
		{
			if (point.x < 0)
				strcpy(point_properties.axis_location, "Negative X");
			if (point.x > 0)
				strcpy(point_properties.axis_location, "Positive X");
			point_properties.quadrant = 0; // A point lying on Any Of the Co-ordinate axis is not a part of any quadrant...
			printf("The Point Lies On The %s Axis !!!\n\n", point_properties.axis_location);
		}
		else // BOTH X AND Y ARE NON-ZERO
		{
			point_properties.axis_location[0] = '\0'; // A Point Lying in any of the 4 Quadrants Cannot  Be Lying On Any Of The Co-ordinate Axes...

			if (point.x > 0 && point.y > 0) // X IS +ve AND y Is +ve 
				point_properties.quadrant = 1;
			else if (point.x < 0 && point.y > 0) // X IS -ve AND y Is +ve 
				point_properties.quadrant = 2;
			else if (point.x < 0 && point.y < 0) // X IS -ve AND y Is -ve 
				point_properties.quadrant = 3;
			else // X IS -ve AND y Is -ve 
				point_properties.quadrant = 4;

			printf("The Point Lies In Quadrant Number %d !!!\n\n", point_properties.quadrant);
		}
	}
	return 0;
}
