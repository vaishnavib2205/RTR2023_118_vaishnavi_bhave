#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

#define NAME_LENGTH 100
#define MARATIAL_STATUS 10

struct Employee
{
	char name[NAME_LENGTH];
	int age;
	char sex;
	float salary;
	char maratial_status;
};

int main(void)
{
	// function prototype
	void MyGetString(char[], int);

	//variable declaration

	struct Employee* pEmployeeRecord = NULL;
	int num_employees, i;

	// code
	printf("\n\n");
	printf("Enter Number of employees whose details you want in the record : ");
	scanf("%d", &num_employees);

	printf("\n\n");
	pEmployeeRecord = (struct Employee*)malloc(sizeof(struct Employee)*num_employees);

	if (pEmployeeRecord == NULL)
	{
		printf("FAILED TO ALLOCATE MEMORY TO 'struct Employee' !!! Exiting NOW...\n\n");
		exit(0);
	}
	else
	{
		printf("SUCCESSFULLY ALLOCATED MEMORY TO 'struct Employee' !!! \n\n");
	}

	// USER input in Array

	for (i = 0; i < num_employees; i++)
	{
		printf("\n\n\n\n");
		printf("****** DATA ENTRY FOR EMPLOYEE NUMBER %d ******\n", (i + 1));
		printf("\n\n");
		printf("Enter Employee Name : ");
		MyGetString(pEmployeeRecord[i].name, NAME_LENGTH);

		printf("\n\n");
		printf("Enter Employee Age : ");
		scanf("%d", &pEmployeeRecord[i].age);

		printf("\n\n");
		printf("Enter Employee's Sex (Male - M Or Female - F) : ");
		pEmployeeRecord[i].sex = getch();
		printf("%c", pEmployeeRecord[i].sex);
		pEmployeeRecord[i].sex = toupper(pEmployeeRecord[i].sex);

		printf("\n\n");
		printf("Enter Employee Salary : ");
		scanf("%f", &pEmployeeRecord[i].salary);

		printf("\n\n");
		printf("Is The EMployee Married? (Y / N) : ");
		pEmployeeRecord[i].maratial_status = getch();
		printf("%c", pEmployeeRecord[i].maratial_status);
		pEmployeeRecord[i].maratial_status = toupper(pEmployeeRecord[i].maratial_status);

	}

	// Display

	printf("\n\n\n\n");
	printf("****** DISPLAYING EMPLOYEE RECORDS ******");
	for (i = 0; i < num_employees; i++)
	{
		printf("****** EMPLOYEE NUMBER %d ******\n\n", (i + 1));
		printf("Name			:%s\n", pEmployeeRecord[i].name);
		printf("Age`			:%d\n", pEmployeeRecord[i].age);

		if (pEmployeeRecord[i].sex == 'M')
			printf("Sex				:Male\n");
		else if (pEmployeeRecord[i].sex == 'F')
			printf("Sex				: FeMale\n");
		else
			printf("Sex				: Invalid Data Entered\n");

		printf("Salary			:%f\n", pEmployeeRecord[i].salary);

		if (pEmployeeRecord[i].maratial_status == 'Y')
			printf("Sex				:Married\n");
		else if (pEmployeeRecord[i].maratial_status == 'N')
			printf("Sex				: Unmarried\n");
		else
			printf("Sex				: Invalid Data Entered\n");

		printf("\n\n");
	}

	if (pEmployeeRecord)
	{
		free(pEmployeeRecord);
		pEmployeeRecord = NULL;
		printf("Memory Allocated To struct Employee Has Been Successfully Freed !!!\n\n");

	}

	return 0;

}

// SIMPLE RUDIMENTARY IMPLIMENTATION OF gets_s() 
// IMPLIMENTATION DUE TO DIFFERENT BEHAVIOUR OF gets_s()
// BACKSPACE / CHARACTER DELETION AND ARROW KEY CURSOUR MOMENT NOPT EMPLEMENTED

void MyGetString(char str[], int str_size)
{
	// variable declaration

	int i;
	char ch = '\0';

	// code
	i = 0;
	do
	{
		ch = getch();
		str[i] = ch;
		printf("%c", str[i]);
		i++;
	} while ((ch != '\r') && (i < str_size));

	if (i == str_size)
		str[i - 1] = '\0';
	else
		str[i] = '\0';
}