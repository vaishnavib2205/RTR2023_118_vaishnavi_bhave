#include <stdio.h>
int main(void){
//variable declarations

	int num;
	int* ptr = NULL;
	int ans;


	//code
	num = 5;
	ptr = &num;
	printf("  num     =%d\n", num);
	printf("  &num     =%p\n", &num);
	printf(" *(&num)      =%d\n", *(&num));
	printf("  ptr     =%d\n", ptr);
	printf("  *ptr     =%p\n", *ptr);

	printf("\n\n");

	printf("Answer of (ptr + 10)  = %p\n", (ptr + 10));


	printf("Answer of *(ptr + 10)  = %p\n", (ptr + 10));

	printf("Answer of (*ptr + 10)  = %p\n", *(ptr + 10));
	
	printf("Answer of (ptr + 10)  = %p\n", (*ptr + 10));
	
	printf("Answer of (ptr + 10)  = %p\n", (*ptr));
	
	printf("Answer of (ptr + 10)  = %p\n", (*ptr));
	
	printf("Answer of (*ptr)++  = %p\n", (*ptr));

	return(0);
}