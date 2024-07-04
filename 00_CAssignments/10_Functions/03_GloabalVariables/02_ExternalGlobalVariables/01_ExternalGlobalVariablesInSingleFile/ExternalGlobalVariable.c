#include<stdio.h>

//GLOBAL SCOPE

int main(void)
{
	//function prototype
	void change_count(void);

	//variable declaration
	extern int global_count; // The extern keyword in C is used to declare a variable as a global variable

	// code
	printf("\n");
	printf("Value of global_count before change_count() = %d\n", global_count);
	change_count();
	printf("Value of global_count after change_count() = %d\n", global_count);
	return 0;
}

// GLOBAL SCOPE
// global_count is a global variable
// since, it is declared before change_count(), it can be accessed and used as any ordinary global variable in change_count()
// since, it is declared after main(), it must be first re-declared in main()as an external global variable by means of the 'extern' keyword and the type of the variable.
//once this is done, it can be used as an ordinary global variable in main as well

int global_count = 0;

void change_count(void)
{
	// code
	global_count = 5;
	printf("Value of global_count in change_count() = %d\n", global_count);
}