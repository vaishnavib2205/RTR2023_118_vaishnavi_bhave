#include<stdio.h>

int main(void)
{
	// variable declaration

	int iArray[] = { 9,30,6,12,98,95,20,23,2,45 };
	int int_size;
	int iArray_size;
	int iArray_num_elements;

	int fArray[] = { 1.2f, 2.3f , 3.4f, 4.5f, 5.6f, 6.7f, 7.8f, 8.9f };
	int float_size;
	int fArray_size;
	int fArray_num_elements;

	int cArray[] = { 'A' , 'S','T', 'R','O','M','E','I','C','O','M','P' };
	int char_size;
	int cArray_size;
	int cArray_num_elements;

	int i;

	// code

	// iArray[]
	printf("\n\n");
	printf("In-line Initialization And Loop (for) Display Of Elements of Array 'iArray[]' : \n\n");

	int_size = sizeof(int);
	iArray_size = sizeof(iArray);
	iArray_num_elements = iArray_size / int_size;

	for (i = 0; i < iArray_num_elements; i++)
	{
		printf("iArray[%d] (Element %d) = %d\n", i, (i + 1), iArray[i]);
	}

	printf("Size Of data type 'int'     = %d bytes\n", int_size);

	printf("Number Of Elements In 'int' Array 'iArray[]'  =%d Element \n", iArray_num_elements);

	printf("Size Of Array 'iArray[]' (%d Elements * %d Bytes)  =%d Bytes\n\n", iArray_num_elements, int_size, iArray_size);

	// fArray[]

	printf("\n\n");
	printf("In-line Initialization And Loop (for) Display Of Elements of Array 'fArray[]' : \n\n");

	float_size = sizeof(float);
	fArray_size = sizeof(fArray);
	fArray_num_elements = fArray_size / float_size;

	for (i = 0; i < fArray_num_elements; i++)
	{
		printf("fArray[%d] (Element %d) = %d\n", i, (i + 1), fArray[i]);
	}

	printf("Size Of data type 'float'     = %d bytes\n", float_size);

	printf("Number Of Elements In 'float' Array 'fArray[]'  =%d Element \n", fArray_num_elements);

	printf("Size Of Array 'fArray[]' (%d Elements * %d Bytes)  =%d Bytes\n\n", fArray_num_elements, float_size, fArray_size);

	// char[]
	printf("\n\n");
	printf("In-line Initialization And Loop (for) Display Of Elements of Array 'cArray[]' : \n\n");


	char_size = sizeof(char);
	cArray_size = sizeof(cArray);
	cArray_num_elements = cArray_size / char_size;

	for (i = 0; i < cArray_num_elements; i++)
	{
		printf("fArray[%d] (Element %d) = %d\n", i, (i + 1), cArray[i]);
	}


	printf("Size Of data type 'char'     = %d bytes\n", char_size);

	printf("Number Of Elements In 'char' Array 'cArray[]'  =%d Element \n", cArray_num_elements);

	printf("Size Of Array 'cArray[]' (%d Elements * %d Bytes)  =%d Bytes\n\n", cArray_num_elements, char_size, cArray_size);

	return 0;
}