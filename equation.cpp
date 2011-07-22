#include "equation.h"
#include "sortedList.h"
#include <string>
#include <iostream>

using namespace std;

float equation::minus(const sortedList& list) const
{
	return (list[f1] - list[f2]);
}

float equation::plus(const sortedList& list) const
{
	return (list[f1] + list[f2]);
}

float equation::nothing(const sortedList& list) const
{
	return (list[f1]);
}

float equation::multiply(const sortedList& list) const
{
	return (list[f1] * list[f2]);
}

float equation::divide(const sortedList& list) const
{
	return (list[f1] / list[f2]);
}

equation::equation(string eq)
{
	f1 = f2 = 1;

	unsigned i;
	int tint;
	bool first = false,
		 second = false,
		 found_op = false;
	string ops = "-+*/";
	
	//look out for F's and arithmetic operators
	for (i = 0; i < eq.size(); ++i)//for each character...
	{
		//check if it's an F
		if (eq[i] == 'F')
		{
			tint = 0;
			//get the next number(s)
			for (++i;i< eq.size() && eq[i] >= '0' && eq[i] <= '9'; ++i)
			{
				tint *= 10;
				tint += int(eq[i]-'0');
			}
			
			--i;
			
			//now that we have a number, figure out where to stick it
			if (!first)
			{
				f1 = tint;
				first = true;
			}
			else
			{
				f2 = tint;
				second = true;
			}
		}
		
		//if it's not an F, it should be an operator
		else if (ops.find(eq[i]) != string::npos)
		{
			found_op = true;
		
			switch (eq[i])
			{
				case '-': func = &equation::minus;		break;
				case '+': func = &equation::plus;		break;
				case '*': func = &equation::multiply;	break;
				case '/': func = &equation::divide;		break;
				default:  cout << "what?" << endl;		break;
			}
		}
		
		//if it's neither, there's a problem
		else
		{
			f1 = 1;
			func = &equation::nothing;
			
			cout << "Warning: unreadable equation." << endl;
		}
	}
	
	//if it's one number alone
	if (first && !found_op)
	{
		func = &equation::nothing;
	}
	
	if (!first || (first && found_op && !second))
	{
		cout << "Warning: missing F distance after operator."<<endl;
		
		f1 = 1;
		func = &equation::nothing;
	}
}

float equation::evaluate(const sortedList& list) const
{
	return ((this->*func)(list));
}

int equation::maxF() const
{
	if (f1 > f2)
		return (f1);
	else
		return (f2);
}
