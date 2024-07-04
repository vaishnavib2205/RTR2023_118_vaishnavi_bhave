#include <stdio.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3
#define DEPTH 2

int main(void) {

//variable declarations
//In-Line initialization

int iArray[NUM_ROWS][NUM_COLUMNS][DEPTH] = { {{9, 18}, {27, 36}, {45, 54}},
								             {{8, 16}, {24, 32}, {40, 48}},
								             {{7, 14}, {21, 28}, {35, 42}},
								             {{6, 12}, {18, 24}, {30, 36}},
								             {{5, 10}, {15, 20}, {25, 30}} };

int i, j, k;

int iArray_1D[NUM_ROWS + NUM_COLUMNS + DEPTH];

//**********Display 3D Array***************

printf("\n\n");
printf("Elements in the 3D array: \n\n");

for (i = 0; i < NUM_ROWS; i++)
{

}
}