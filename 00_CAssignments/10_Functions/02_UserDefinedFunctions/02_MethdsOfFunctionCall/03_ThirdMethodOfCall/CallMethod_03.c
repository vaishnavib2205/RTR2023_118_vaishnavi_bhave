#include<stdio.h>

int main(int argc, char* argv[], char* envp[]) {

//function prototypes
	void Function_Country();

	//code
	Function_Country();
	return(0);
}

void Function_Country(void) {

//function declaration
	void Function_OfAMC(void);

	//code
	Function_OfAMC();

	printf("\n\n");
	printf("I live in India");
	printf("\n\n");
}
void Function_OfAMC(void) {
	void Function_SurName(void);

	//code
	Function_SurName();
	printf("\n\n");
	printf("Of AtsroMediComp");
}

void Function_SurName(void)//funstion definition
{

	void Function_MiddleName(void);

	//code
	Function_MiddleName();
	printf("\n\n");
	printf("Bhave");
}

void Function_MiddleName(void) {

	void Function_FirstName(void);

	//code
	Function_FirstName();

	printf("\n\n");
	printf("Bhalchandra");
}

void Function_FirstName(void) {

	void Function_Is(void);

	Function_Is();
	printf("\n\n");
	printf("Vaishnavi");
}

void Function_Is(void) {
	void Function_Name(void);

	Function_Name();

	printf("\n\n");
	printf("Is");
}

void Function_Name(void) {
	void Function_My();
	printf("\n\n");
	printf("Name");
}

void Function_My(void) {

//code
	printf("\n\n");
	printf("My");
}
}