#include <stdio.h>

struct Rectangle
{
	struct MyPoint {
		int x;
		int y;
	} point_01, point_02;

} rect;
int main(void) {
	//variable declaration
	int length, breadth, area;

	//code
	printf("\n\n");
	printf("Enter leftmost X-coordinate of rectangle: ");
	scanf("%d", &rect.point_01.x);

	printf("\n\n");
	printf("Enter Bottommost Y-coordinate of rectangle: ");
	scanf("%d", &rect.point_01.y);

	printf("\n\n");
	printf("Enter leftmost X-coordinate of rectangle: ");
	scanf("%d", &rect.point_02.x);

	printf("\n\n");
	printf("Enter Rightmost Y-coordinate of rectangle: ");
	scanf("%d", &rect.point_02.y);

	length = rect.point_02.y - rect.point_01.y;

	if (length < 0)
		length = length * -1;
	breadth = rect.point_02.x - rect.point_01.x;
	if (breadth < 0)
		breadth = breadth * -1;

	area = length * breadth;

	printf("\n\n");
	printf("Length of Rectangle = %d\n\n", length);
	printf("Breadth of rectangle = %d\n\n", breadth);
	printf("Area of Rectangle = %d\n\n", area);

	return(0);
}