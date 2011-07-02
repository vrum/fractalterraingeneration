#include "sortedList.h"
#include <iostream>
#include <cfloat>

using namespace std;

sortedList::sortedList(int size)
{
	if (size < 1)
		cout << "too small!"<<endl;

	data = new float[size];
	
	for (s = 0; s < size; ++s)
		data[s] = FLT_MAX;
}

sortedList::~sortedList()
{
	delete[] data;
}

float sortedList::operator[](int index) const
{
	if (index <= s && index > 0)
		return (data[index-1]);
	else if (index == 0)
	{
		cout << "warning: using zero index!" << endl;
		return (data[0]);
	}
	else
	{
		cout << "bad index"<<endl;
		return (FLT_MAX);
	}
}

void sortedList::operator+=(float val)
{
	for (int i = 0; i < s; ++i)
	{
		//check if it's smaller
		if (val < data[i])
		{
			//swap 'em
			float tv = val,tv2;
			
			for (int j = i; j < s; ++j)
			{
				tv2 = data[j];
				data[j] = tv;
				tv = tv2;
			}
			
			break;
		}	
	}
}

void sortedList::clear()
{
	for (int i = 0; i < s; ++i)
		data[i] = FLT_MAX;
}
