#include<stdio.h>
#include<ctype.h>

#define NUM_EMPLOYEES 5 // Simply change this constant value to have as many number of employee records as you please...

#define NAME_LENGTH 100
#define MARITAL_STATUS 10

struct Employee
{
	char name[NAME_LENGTH];
	int age;
	float salary;
	char sex;
	char maritial_status;
};

int main(void)
{
	// function prototype
	void MyGetString(char[], int);

	// variable declarations

	struct Employee EmployeeRecord[NUM_EMPLOYEES]; // AnArray of 5 structure
	int i;

	// code
	// ******* USER INPUT INITIALIZATION OF ARRAY ******

	for (i = 0; i < NUM_EMPLOYEES; i++)
	{
		printf("\n\n");
		printf("****** DATA ENTRY FOR EMPLOYEE NUMBER %d ******\n\n", (i + 1));

		printf("\n\n");
		printf("Enter Employee Name : ");
		MyGetString(EmployeeRecord[i].name, NAME_LENGTH);

		printf("\n\n");
		printf("Enter Employee Age : ");
		scanf("%d", &EmployeeRecord[i].age);

		printf("\n\n");
		printf("Enter Employee's sex : ");
		EmployeeRecord[i].sex = getch();
		printf("%c", EmployeeRecord[i].sex);
		EmployeeRecord[i].sex = toupper(EmployeeRecord[i].sex);

		printf("\n\n\n");
		printf("Enter Employee's Salary (In INDIAN Rupee) : ");
		scanf("%f", &EmployeeRecord[i].salary);

		printf("\n\n");
		printf("Is The Employee Married? : ");
		EmployeeRecord[i].maritial_status = getch();
		printf("%c", EmployeeRecord[i].maritial_status);
		EmployeeRecord[i].maritial_status = toupper(EmployeeRecord[i].maritial_status);

	}

	// DISPLAY
	printf("\n\n\n");
	printf("******* DISPLAYING EMPLOYEE RECORDS ******* \n\n");

	for (i = 0; i < 5; i++)
	{
		printf("****** EMPLOYEE NUMBER %d ******\n\n", (i + 1));
		printf("Name				: %s\n", EmployeeRecord[i].name);
		printf("Age					: %d years\n", EmployeeRecord[i].age);

		if (EmployeeRecord[i].sex == 'M')
		{
			printf("Sex				: Male\n");
		}
		else if(EmployeeRecord[i].sex == 'F')
		{
			printf("Sex				: Female\n");
		}
		else
		{
			printf("Sex				: Invalid Data Entered\n");
		}

		printf("Salary				: Rs. %f\n", EmployeeRecord[i].salary);

		if (EmployeeRecord[i].maritial_status == 'Y')
		{
			printf("Maratial Status			: Married\n");
		}
		else if (EmployeeRecord[i].maritial_status == 'N')
		{
			printf("Maratial Status			: Unmarried\n");
		}
		else
		{
			printf("Maratial Status			: Invalid Data Entered\n");
		}
		printf("\n\n");
	}
	return 0;

}

void MyGetString(char str[], int str_size)
{
	// variable declarations
	int i;
	char ch = '\0';

	//code

	i = 0;
	do
	{
		ch = getch();
		str[i] = ch;
		printf("%c", str[i]);
		i++;
	} while ((ch != '\r') && (i < str_size));

	if (i == str_size)
	{
		str[i - 1] = '\0';
	}
	else
	{
		str[i] = '\0';
	}
}