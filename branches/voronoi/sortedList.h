#ifndef SORTEDLIST_H
#define SORTEDLIST_H

class sortedList
{
	float* data;
	int s;
	
public:
	sortedList(int size);
	~sortedList();
	float operator[](int index) const;
	void operator+=(float val);
	void clear();
};

#endif
