#include<stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {

//variable declarations
	int i;
	//code

	if (argc != 4) //program name + name + first name + middle name + surname = 4 command line arguments are required
	{
		printf("\n\n");
		printf("Invalid Usage!!! Exitting Now...\n\n");
		printf("Usage: CommandLineArgumentsApplication <first name> <middle name> <surname>\n\n");
		exit(0);

	}

	//***this programs prints your full name as entered in the command line arguments ***

	printf("\n\n");
	printf("Your Full Name is: ");

	for (i = 1; i < argc; i++) {
		printf("%s",argv[i]);

	}
	printf("\n\n");
	return(0);
	
	}