#include <stdio.h>

int main(void) {
	int num = 5;
	const int* const ptr = &num;
	//read this line from right to left => ptr is a constant (const) pointer (*) to integer (int) constant (const)."


	//code
	printf("\n");
	printf("Current value of 'num' = %d\n", num);
	printf("Current 'ptr' (Address of 'num') = %p\n", ptr);

	//The following line does not give error... as we are modifying the value of the variable individually

	num++;
	printf("After num++, value of 'num' =%d\n", num);

	//The following line gives error and is hence commented out.
	//We cannot alter the 'ptr' value as 'ptr' is "a constant pointer to constant integer".
	//with respect to the pointer the value it points to is constant and the pointer itself is also constant
	//Uncomment it and see the error
	
	return(0);
}