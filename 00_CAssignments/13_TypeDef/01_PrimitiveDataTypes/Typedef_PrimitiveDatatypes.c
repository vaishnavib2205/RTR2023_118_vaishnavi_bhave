#include <stdio.h>

//global typedef
typedef int MY_INT;

int main(void) {
	//function prototype
	MY_INT Add(MY_INT, MY_INT);

	//typedef
	typedef int MY_INT;
	typedef float PVG_FLOAT;
	typedef char CHARACTER;
	typedef double MY_DOUBLE;

	//just like in win32SDK
	typedef unsigned int UINT;
	typedef UINT HANDLE;
	typedef HANDLE HWND;
	typedef HANDLE HINSTANCE;

	//variable declarations
	MY_INT a = 10, i;
	MY_INT iArray[] = { 9, 18, 27, 36, 45, 54, 63, 72, 81, 90 };

	PVG_FLOAT f_pvg = 30.9f;
	const PVG_FLOAT f_pvg_pi = 3.14f;

	CHARACTER ch = '*';
	CHARACTER chArray_01[] = "Hello";
	CHARACTER chArray_02[][10] = { "RTR", "Batch", "2020-21" };

	MY_DOUBLE d = 8.041997;

	//******just random numbers they have nothing to do with any window's handle or instance handle!! this is just understanding...

	UINT uint = 3456;
	HANDLE handle = 987;
	HWND hwnd = 9876;
	HINSTANCE hInstance = 14466;

	//code
	printf("\n\n");
	printf("Type MY_INT variable a = %d\n", a);
	printf("\n\n");

	for (i = 0; i < (sizeof(iArray) / sizeof(int)); i++);

	{
		printf("Type MY_INT array variable iArray[%d] = %d\n", i, iArray[i]);
	}
	printf("\n\n");


}

