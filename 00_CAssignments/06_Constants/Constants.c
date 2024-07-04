#include <stdio.h>

#define MY_PI 3.1415926535897932

#define AMC_String "AstroMediComp RTR"

enum
{
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

enum {
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

//named enums

enum Numbers {
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN
};

enum boolean {
	TRUE = 1,
	FALSE = 0
};

int main(void)
{
	//local constant declarations
	const double epsilon = 0.000001;

	//code
	printf("\n\n");
	printf("Local Constant Epsilon =%lf\n\n", epsilon);

	printf("Sunday Is Day Number = %d\n", SUNDAY);
	printf("Monday Is Day Number = %d\n", MONDAY);
	printf("Tuesday Is Day Number = %d\n", TUESDAY);
	printf("Wednesday Is Day Number = %d\n", WEDNESDAY);
	printf("Thursday Is Day Number = %d\n", THURSDAY);
	printf("Friday Is Day Number = %d\n", FRIDAY);
	printf("Saturday Is Day Number = %d\n", SATURDAY);

	printf("One Is Enum Number =%d/n", ONE);
	printf("Two Is Enum Number =%d/n", TWO);
	printf("Three Is Enum Number =%d/n", THREE);
	printf("Four Is Enum Number =%d/n", FOUR);
	printf("Five Is Enum Number =%d/n", FIVE);
	printf("Six Is Enum Number =%d/n", SIX);
	printf("Seven Is Enum Number =%d/n", SEVEN);
	printf("Eight Is Enum Number =%d/n", EIGHT);
	printf("Nine Is Enum Number =%d/n", NINE);
	printf("Ten Is Enum Number =%d/n", TEN);

	printf("January Is Month Number = %d\n", JANUARY);
	printf("February Is Month Number = %d\n", FEBRUARY);
	printf("March Is Month Number = %d\n", MARCH);
	printf("April Is Month Number = %d\n", APRIL);
	printf("May Is Month Number = %d\n", MAY);
	printf("June Is Month Number = %d\n", JUNE);
	printf("July Is Month Number = %d\n", JULY);
	printf("August Is Month Number = %d\n", AUGUST);
	printf("September Is Month Number = %d\n", SEPTEMBER);
	printf("October Is Month Number = %d\n", OCTOBER);
	printf("November Is Month Number = %d\n", NOVEMBER);
	printf("December Is Month Number = %d\n", DECEMBER);

	printf("Value Of TRUE Is = %d/n", TRUE);
	printf("Value Of FALSE Is = %d/n", FALSE);

	printf("MY_PI Macro Value = %101f\n\n", MY_PI);
	printf("Area Of Circle Of Radius 2 Units = %f\n\n", (MY_PI * 2.0f * 2.0f));

	printf("\n\n");

	printf(AMC_String);
	printf("\n\n");

	printf("AMC_String is : %s\n", AMC_String);
	printf("\n\n");

	return(0);

}
