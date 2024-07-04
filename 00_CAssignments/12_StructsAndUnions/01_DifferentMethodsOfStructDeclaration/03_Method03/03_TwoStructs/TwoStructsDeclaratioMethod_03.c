include <stdio.h>

int main(void) {
	//defining struct
	struct MyPoint
	{
		int x;
		int y;

	}point; //declaring a single variable of type struct mypoint globally

//defining stuct

	struct MyPointPrperties {
		int quadrant;
		char axis_location[10];
	}
	point properties;
	//declaraing a single variable of type struct MyPointProeprties globally...

//code
//user input for the data members of struct mypoint variable point_A 
	printf("\n\n");
	printf("Enter X-Coordinate for A point: ");
	scanf("%d", &point.x);
	printf("Enter Y-Coordinate for A point: ");
	scanf("%d", &point.y);

	printf("\n\n");
	printf("Point Co-ordinates (x, y) are: (%d, %d)!!!\n\n", point.x, point.y);

	if (point.x == 0 && point.y == 0)//if x is zero obviously x or y is the non zero value ....
		printf("The point is the origin (%d, %d)!!!\n", point.x, point.y);
	else {
		if (point.x == 0) {
			if (point.y < 0)
				strcpy(point_properties.axis_location, "Negative Y");

			if (point.y > 0)
				strcpy(point_properties.axis_location, "Positive Y");

			point_properties.quadrant = 0;
			printf("The point lies on a %s axis!!!\n\n", point_properties.axis_location);
		}
		else if (point.y == 0) {
			if (point.x < 0)
				strcpy(point_proeprties.axis_location, "Negative X");

			if (point.x > 0)
				strcpy(point_properties.axis_location, "Positive X");

			point_properties.quadrant = 0;

			printf("The point lies on the %s axis!!!\n\n");
		}
		else {
			point_properties.axis_location[0] = '\0';

			if (point.x > 0 && point.y > 0)
				point_properties.quadrant = 1;
			else if (point.x < 0 && point.y > 0)
				point_properties.quadrant = 2;
			else if (point.x < 0 && point.y < 0)
				point_properties.quadrant = 3;

			else
				point_properties.quadrant = 4;

			printf("The point lies in quadrant number %d !!!\n\n", point_properties.quadrant);
		}
	}
	return(0);
}

}