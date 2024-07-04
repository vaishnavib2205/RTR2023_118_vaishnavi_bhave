#include<stdio.h> // Printf()

// user defined functions : method of calling function 2

//Calling only two functions directly in main(), REST OF THE FUNCTION TRACE THEIR CALL INDIRECTLY TO main()

int main(int argc,  char *argv[], char *envp[])
{
	//function prototypes
	void display_information(void);
	void Function_Country(void);

	//code

	display_information(); //function call
	Function_Country(); //function call

	return(0);

}

// User defined functions definations

void display_information(void) // function defination 
{
	//function prototypes declaration

	void Function_My(void);
	void Function_Name(void);
	void Function_Is(void);
	void Function_FirstName(void);
	void Function_MiddleName(void);
	void Function_Surname(void);
	void Function_OfAMC(void);

	//code

	// Function Calls

	Function_My();
	Function_Name();
	Function_Is();
	Function_FirstName();
	Function_MiddleName();
	Function_Surname();
	Function_OfAMC();
}

void Function_My(void) // Function Definition
{
	//code
	printf("\n\n");
	printf("My");
}

void Function_Name(void)
{
	//code
	printf("\n\n");
	printf("Name");
}

void Function_Is(void)
{
	//code
	printf("\n\n");
	printf("Is");
}

void Function_FirstName(void)
{
	printf("\n\n");
	printf("Vaishnavi");
}

void Function_MiddleName(void) // Function Definition
{
	printf("\n\n");
	printf("Bhalchandra");
}

void Function_Surname(void) // Function Definition
{
	printf("\n\n");
	printf("Bhave");
}

void Function_OfAMC(void) // Function Definition
{
	printf("\n\n");
	printf("Of AstroMediComp");
}

void Function_Country(void) // Function Definition
{
	printf("\n\n");

	printf("I Live In India");

	printf("\n\n");
}