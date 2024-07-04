#include <stdio.h>

int main(void)
{
	char cArray[10];
	int i;

	//code
	for (i = 0; i < 10; i++)
		cArray[i] = (char)(i + 65);
	

	printf("\n\n");
	printf("Elements of the 'Character' Array: \n\n");
	for (i = 0; i < 10; i++)
		printf("cArray[%d] = %c\n", i, cArray[i]);

	printf("\n\n");
	printf("Elements of the 'character' Array: \n\n");
	for (i = 0; i < 10; i++)
		printf("cArray[%d] = %c \t \t Address = %p\n", i, cArray[i], &cArray[i]);
	printf("\n\n");
	return(0);
}