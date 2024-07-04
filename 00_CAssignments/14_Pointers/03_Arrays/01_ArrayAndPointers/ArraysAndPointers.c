#include <stdio.h>

int main(void)
{
	//variable declaration
	int iArray[] = { 12, 24, 36, 48, 60, 72, 84, 96, 108, 120 };
	float fArray[] = { 9.8f, 8.7f, 7.6f, 6.5f, 5.4f };
	double dArray[] = { 1.222222, 2.333333, 3.444444 };
	char cArray[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P', '\0' };

	//code
	printf("\n\n");
	printf("Integer Array Elements And the addresses the occupy are as follows: \n\n");
	printf("iArray[0] = %d  \t At Address : %p\n", *(iArray + 0), (iArray + 0));
	printf("iArray[1] = %d  \t At Address : %p\n", *(iArray + 1), (iArray + 1));
	printf("iArray[2] = %d  \t At Address : %p\n", *(iArray + 2), (iArray + 2));
	printf("iArray[3] = %d  \t At Address : %p\n", *(iArray + 3), (iArray + 3));
	printf("iArray[4] = %d  \t At Address : %p\n", *(iArray + 4), (iArray + 4));
	printf("iArray[5] = %d  \t At Address : %p\n", *(iArray + 5), (iArray + 5));
	printf("iArray[6] = %d  \t At Address : %p\n", *(iArray + 6), (iArray + 6));
	printf("iArray[7] = %d  \t At Address : %p\n", *(iArray + 7), (iArray + 7));
	printf("iArray[8] = %d  \t At Address : %p\n", *(iArray + 8), (iArray + 8));
	printf("iArray[9] = %d  \t At Address : %p\n", *(iArray + 9), (iArray + 9));

	printf("\n\n");
	printf("Float array elements and the addresses they occupy are as follows: \n\n");
	printf("fArray[0] = %f  \t At Address : &p\n", *(fArray + 0), (fArray + 0));
	printf("fArray[0] = %f  \t At Address : &p\n", *(fArray + 1), (fArray + 1));
	printf("fArray[0] = %f  \t At Address : &p\n", *(fArray + 2), (fArray + 2));
	printf("fArray[0] = %f  \t At Address : &p\n", *(fArray + 3), (fArray + 3));
	printf("fArray[0] = %f  \t At Address : &p\n", *(fArray + 4), (fArray + 4));


	printf("dArray[0] = %f  \t At Address : &p\n", *(dArray + 0), (dArray + 0));
	printf("dArray[0] = %f  \t At Address : &p\n", *(dArray + 1), (dArray + 1));
	printf("dArray[0] = %f  \t At Address : &p\n", *(dArray + 2), (dArray + 2));


	printf("fArray[0] = %f  \t At Address : &p\n", *(cArray + 0), (cArray + 0));
	printf("fArray[1] = %f  \t At Address : &p\n", *(cArray + 1), (cArray + 1));
	printf("fArray[2] = %f  \t At Address : &p\n", *(cArray + 2), (cArray + 2));
	printf("fArray[3] = %f  \t At Address : &p\n", *(cArray + 3), (cArray + 3));
	printf("fArray[4] = %f  \t At Address : &p\n", *(cArray + 4), (cArray + 4));
	printf("fArray[5] = %f  \t At Address : &p\n", *(cArray + 5), (cArray + 5));
	printf("fArray[6] = %f  \t At Address : &p\n", *(cArray + 6), (cArray + 6));
	printf("fArray[7] = %f  \t At Address : &p\n", *(cArray + 7), (cArray + 7));
	printf("fArray[8] = %f  \t At Address : &p\n", *(cArray + 8), (cArray + 8));
	printf("fArray[9] = %f  \t At Address : &p\n", *(cArray + 9), (cArray + 9));
	printf("fArray[10] = %f  \t At Address : &p\n", *(cArray + 10), (cArray + 10));
	printf("fArray[11] = %f  \t At Address : &p\n", *(cArray + 11), (cArray + 11));
	printf("fArray[12] = %f  \t At Address : &p\n", *(cArray + 12), (cArray + 12));
	printf("fArray[13] = %f  \t At Address : &p\n", *(cArray + 13), (cArray + 13));

	return(0);



}