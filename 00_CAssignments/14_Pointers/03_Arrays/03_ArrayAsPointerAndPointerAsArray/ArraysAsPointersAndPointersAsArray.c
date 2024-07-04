#include <stdio.h>

int main(void) {
	//variable declarations
	int iArray[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
	int* ptr_iArray = NULL;

	//code
	//#### Using Array Name As pointer i.e. Value of the xth element of iArray: *(iArray + x) And Address of xth element of iArray : (iArray + x) ##############
	printf("\n\n");
	printf("***Using Array Name AS Pointer i.e: Value of xth element of iArray:  *(iArray + x) and address of xth element of xth element of iArray: (iArray + x) *******\n\n");
	printf("*(iArray + 0) = %d  \t at address (iArray + 0) : %p\n", *(iArray + 0), (iArray + 0));
	printf("*(iArray + 1) = %d  \t at address (iArray + 1) : %p\n", *(iArray + 1), (iArray + 1));
	printf("*(iArray + 2) = %d  \t at address (iArray + 2) : %p\n", *(iArray + 2), (iArray + 2));
	printf("*(iArray + 3) = %d  \t at address (iArray + 3) : %p\n", *(iArray + 3), (iArray + 3));
	printf("*(iArray + 4) = %d  \t at address (iArray + 4) : %p\n", *(iArray + 4), (iArray + 4));
	printf("*(iArray + 5) = %d  \t at address (iArray + 5) : %p\n", *(iArray + 5), (iArray + 5));
	printf("*(iArray + 6) = %d  \t at address (iArray + 6) : %p\n", *(iArray + 6), (iArray + 6));
	printf("*(iArray + 7) = %d  \t at address (iArray + 7) : %p\n", *(iArray + 7), (iArray + 7));
	printf("*(iArray + 8) = %d  \t at address (iArray + 8) : %p\n", *(iArray + 8), (iArray + 8));
	printf("*(iArray + 9) = %d  \t at address (iArray + 9) : %p\n", *(iArray + 9), (iArray + 9));

	//Assigning base address of Integer array 'iArray' to integer pointer 'ptr_iArray'
	//Name of any array is its own base address

	ptr_iArray = iArray;

	printf("\n\n");

	printf("********Using pointer as array name i.e. : Value of xth element of iArray : ptr_iArray[x] and Address of xth element of iArray : &ptr_iArray[x] ******\n\n");
	printf("Integer Array Element And their Addresses: \n\n");
	printf("*(iArray + 0) = %d \t at address (iArray + 0) : %p\n", *(iArray + 0), (iArray + 0));
	printf("*(iArray + 1) = %d \t at address (iArray + 1) : %p\n", *(iArray + 1), (iArray + 1));
	printf("*(iArray + 2) = %d \t at address (iArray + 2) : %p\n", *(iArray + 2), (iArray + 2));
	printf("*(iArray + 3) = %d \t at address (iArray + 3) : %p\n", *(iArray + 3), (iArray + 3));
	printf("*(iArray + 4) = %d \t at address (iArray + 4) : %p\n", *(iArray + 4), (iArray + 4));
	printf("*(iArray + 5) = %d \t at address (iArray + 5) : %p\n", *(iArray + 5), (iArray + 5));
	printf("*(iArray + 6) = %d \t at address (iArray + 6) : %p\n", *(iArray + 6), (iArray + 6));
	printf("*(iArray + 7) = %d \t at address (iArray + 7) : %p\n", *(iArray + 7), (iArray + 7));
	printf("*(iArray + 8) = %d \t at address (iArray + 8) : %p\n", *(iArray + 8), (iArray + 8));
	printf("*(iArray + 9) = %d \t at address (iArray + 9) : %p\n", *(iArray + 9), (iArray + 9));

	//Assigning the base address of integer array 'iArray' to integer pointer 'ptr_iArray'
	//Name of any array is its own base address

	ptr_iArray = iArray;

	//Using Pointer As array Name i.e. : Value f xth Element of iArray: ptr_iArray[x] and addresses of xth element of iArray : &pyr_iArray[x] #####
	printf("\n\n");
	printf("******Using Pointer As Array Name i.e.: Value of xth element of iArray:  ptr_iArray[x] and address of xth element of iArray: &ptr_iArray[x] **********\n\n");
	printf("Integer Array Elements And Their Addresses : \n\n");
	printf("ptr_iArray[0] = %d  \t at address &ptr_iArray[0] : %p\n", ptr_iArray[0], &ptr_iArray[0]);
	printf("ptr_iArray[1] = %d  \t at address &ptr_iArray[1] : %p\n", ptr_iArray[1], &ptr_iArray[1]);
	printf("ptr_iArray[2] = %d  \t at address &ptr_iArray[2] : %p\n", ptr_iArray[2], &ptr_iArray[2]);
	printf("ptr_iArray[3] = %d  \t at address &ptr_iArray[3] : %p\n", ptr_iArray[3], &ptr_iArray[3]);
	printf("ptr_iArray[4] = %d  \t at address &ptr_iArray[4] : %p\n", ptr_iArray[4], &ptr_iArray[4]);
	printf("ptr_iArray[5] = %d  \t at address &ptr_iArray[5] : %p\n", ptr_iArray[5], &ptr_iArray[5]);
	printf("ptr_iArray[5] = %d  \t at address &ptr_iArray[5] : %p\n", ptr_iArray[6], &ptr_iArray[6]);
	printf("ptr_iArray[6] = %d  \t at address &ptr_iArray[6] : %p\n", ptr_iArray[7], &ptr_iArray[7]);
	printf("ptr_iArray[7] = %d  \t at address &ptr_iArray[7] : %p\n", ptr_iArray[8], &ptr_iArray[8]);
	printf("ptr_iArray[8] = %d  \t at address &ptr_iArray[8] : %p\n", ptr_iArray[9], &ptr_iArray[9]);

	return(0);
}