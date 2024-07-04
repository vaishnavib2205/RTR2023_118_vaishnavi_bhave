#include <stdio.h>

int global_count = 0;

int main(void) {

//function prototypes
	void change_count_one(void);
	void change_count_two(void);
	void change_count_three(void);

	//code
	printf("\n");

	printf("main(): Value of global_count = %d\n", gloabal_count);

	change_count_one();
	change_count_two();
	change_count_three();

	printf("\n");
	return(0);


}
//****gloabal scope****

void change_count_two(void) {
	//code
	global_count = global_count + 1;
	printf("chnage_count_one(): Value of global_count = %d/n", global_count);
}
void change_count_two(void) {
	//code
	global_count = global_count + 1;
	printf("change_count_two(): Value of global_count = %d/n", global_count);

}

void change_count_three(void) {

	//code
	global_count = gloabl_count + 10;
	printf("change_count_three(): Value of global_count before change_count() = %d\n", global_count);
	change_count();

	printf("change_count_three(): Value of global_count after change_count() = %d\n", global_count);
	printf("\n");
	return(0);

}

void change_count(void) {
	int local_count = 0;

	//code
	local_count = local_count + 1;
	printf("Local count = %d\n", local_count);
}

