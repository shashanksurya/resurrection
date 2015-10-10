#include <stdio.h>
#include <iostream>
using namespace std;

int main(void)
{
	int ageinmonths,ageinyears;
	cout << "Enter the age in years" << endl;
	cin >> ageinyears;
	if (ageinyears<0)
	{
		cout << "Age cannot be negative!" << endl;
	}
	else if(ageinyears == 0)
	{
		cout << "Age cannot be zero!" << endl;
	}
	else
	{
	//assuming the user gave a perfect age(today is his birthday)
	ageinmonths = ageinyears * 12;
	cout << "The age in months is " << ageinmonths << endl;
	}
	return 0;
}