#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	//variable declarations
	int i;
	int num;
	int sum = 0;

	//code
	if (argc == 1) {
		printf("\n\n");
		printf("N numbers given for addition!!! Exitting now...\n\n");
		printf("Usage: CommandLineArgumentsApplication <first number> <second number>...\n\n");
		exit(0);
	}

	//*** this programs adds all command line argument given in integer form only and outputs the sum***
	//*** due to use of atoi(), all command line arguments f types other than int are ignored***
	//due to use of atoi() all command line arguments of types other than int are ignored

	printf("\n\n");
	printf("Sum of integer command line arguments is : \n\n");

	for (i = 1; i < argc; i++) //loop starts from i =1 because, i =0 will result in argv[i] which is the name of the program itself 

	{
		num = atoi(argv[i]);
		sum = sum + num;
	}
	printf("Sum = %d\n\n", sum);

	return(0);
}
