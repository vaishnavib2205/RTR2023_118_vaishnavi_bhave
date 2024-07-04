#include <stdio.h>

int main(void)
{
	int i, j;
	char ch_01, ch_02;
	int a, result_int;
	float f, result_float;

	int i_explicit;
	float f_explicit;

	//code
	printf("\n\n");

	//interconversion and implicit type casting between char and int types
	i = 70;
	ch_01 = i;
	printf("Character 1 (after ch_01 = i)= %c\n\n", ch_01);

	ch_02 = 'Q';
	j = ch_02;

	printf("Character 2 = %c\n", ch_02);
	printf("J (after j = ch_02)= %d\n\n", j);

	//implicit conversion of int to float
	a = 5;
	f = 7.8f;
	result_float = a + f;
	printf("Integer a =%d and floating point number %f added gives floating point sum =%f\n\n", a, f, result_float);
	printf("Integer a =%d and floating point number %f added gives Integer sum =%f\n\n", a, f, result_int);

	//Explicit type casting using cast operator
	f_explicit = 30.121995f;
	i_explicit = (int)f_explicit;
	printf("Floating point number which will be type casted explicitly =%f\n", f_explicit);
	printf("Resultant integer after explicit type casting of %f =5d\n\n", f_explicit, i_explicit);

	return(0);

}