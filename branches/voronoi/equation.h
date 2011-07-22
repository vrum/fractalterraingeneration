#ifndef EQUATION_H
#define EQUATION_H

#include <string>
#include "sortedList.h"

class equation
{
	int f1,f2;
	
	float minus		(const sortedList& list) const;
	float plus 		(const sortedList& list) const;
	float nothing	(const sortedList& list) const;
	float multiply	(const sortedList& list) const;
	float divide	(const sortedList& list) const;
	
	float (equation::*func)(const sortedList& list) const;

public:
	equation(std::string eq);
	float evaluate(const sortedList& list) const;
	int maxF() const;
};
#endif
