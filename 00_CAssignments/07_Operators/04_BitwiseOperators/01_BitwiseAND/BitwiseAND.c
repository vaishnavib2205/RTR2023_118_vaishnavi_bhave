#include <stdio.h>

int main(void)
{
	//function prototypes
	void PrintBinaryFormOfNumber(unsigned int);

	//variable declaration
	unsigned int a;
	unsigned int b;
	unsigned int result;

	//code
	printf("\n\n");
	printf("Enter an integer= ");
	scanf("%u", &a);

	printf("\n\n");
	printf("Enter another integer = ");
	scanf("%u", &b);

	printf("\n\n\n\n");
	result = a & b;
	printf("Bitwise AND-ing of \nA = %d (Decimal) and B = %d (Decimal) gives result %d (Decimal). \n\n", a, b, result);

	PrintBinaryFormOfNumber(a);
	PrintBinaryFormOfNumber(b);
	PrintBinaryFormOfNumber(result);

	return(0);
}

//***** Begginers to C programming laguage : Please ignore the code of the fllowing function snippet 'PrintBinaryFormOfNumber()'*****
//You may come back to this code and will understand it much better after you have covered : Array, Loops and function *****
//***** The only objective of writing this function was to ibtain the binary representative of decimal integers so that bitwise AND-ing, OR-ing, Complement and Bit-shifting culd be understood with great case *****


void PrintBinaryFormOfNumber(unsigned int decimal_number)
{
	//variable declarations
	unsigned int quotient, remainder;
	unsigned int num;
	unsigned int binary_array[8];
	int i;

	//code
	for (i = 0; i < 8; i++)
		binary_array[i] = 0;

	printf("The binary form of the decimal integer %d is\t=t", decimal_number);
	num = decimal_number;
	i = 7;

	while (num != 0) {
		quotient = num / 2;
		remainder = num % 2;
		binary_array[i] = remainder;
		num = quotient;
		i--;
	}
	for (i = 0; i < 8; i++)
		printf("%u", binary_array[i]);
	printf("\n\n");
}