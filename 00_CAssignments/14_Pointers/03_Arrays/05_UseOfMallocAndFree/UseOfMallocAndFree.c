#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int* ptr_iArray = NULL; //It is good discipline to initialize any pinter with null address to prevent any garbage value getting into it that way, it makes it easy to check for success or failure of memory allocation later on after malloc

	unsigned int intArrayLength = 0;
	int i;
	printf("\n\n");
	printf("Enter the number of elements you want in your integer array: ");
	scanf("%d", &intArrayLength);
}