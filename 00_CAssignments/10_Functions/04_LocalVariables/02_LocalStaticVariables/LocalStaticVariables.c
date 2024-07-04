#include<stdio.h>

// GLOBAL SCOPE 

int main(void)
{
	// LOCAL SCOPE OF main()  begins

	//variable declaration 
	// 'a' is a local variable. it is local to main() only

	int a = 5;

	//function prototypes

	void change_count(void);

	// code
	printf("\n");
	printf("A = %d\n\n", a);

	//local_count is initialized to 0;
	// local_count = local_count + 1 = 0+1 = 1

	change_count();

	//Since local_count is an ordinary local variable of change_count(), it will not retain its value from previous call to change_count().
	// So local_count is AGAIN initialized to 0
	// local_count = local_count + 1 = 1 + 1 = 2 
	change_count();

	//Since local_count is an ordinary local variable of change_count(), it will not retain its value from previous call to change_count().
// So local_count is AGAIN initialized to 0
// local_count = local_count + 1 = 2 + 1 = 3 
	change_count();

	return 0;
}

// GLOBAL SCOPE

void change_count(void)
{
	// LOCAL SCOPE Of change_count() begins

	//variable declarations
	// local_count is local Static variable it is local to change_count() only.
	static int local_count = 0;

	//code
	local_count = local_count + 1;
	printf("Local Count = %d\n", local_count);

	// LOCAL SCOPE OF change_count() ends
}

//GLOBAL SCOPE