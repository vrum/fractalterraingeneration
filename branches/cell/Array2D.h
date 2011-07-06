#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <string>
#include <iostream>

template <class T>
class Array_2D{
protected:
	T* values;
	unsigned xgrid;
	unsigned ygrid;

public:
	Array_2D(unsigned width, unsigned height):xgrid(width),ygrid(height) {values = new T[ width * height ];}
	Array_2D(unsigned width, unsigned height, T default_value);
	Array_2D(const Array_2D<T>& rhs);
	
	T& operator()(int x, int y);
	const T& operator()(int x, int y) const;

	Array_2D<T>& operator=( const Array_2D<T>& rhs );
	virtual Array_2D<T>& operator+=( const Array_2D<T>& rhs );
	virtual Array_2D<T>& operator-=( const Array_2D<T>& rhs );
	virtual const Array_2D operator+( const Array_2D<T>& rhs ) const {return (Array_2D(*this)+=rhs);}
	virtual const Array_2D operator-( const Array_2D<T>& rhs ) const {return (Array_2D(*this)-=rhs);}
	
	unsigned width(){return xgrid;}
	unsigned height(){return ygrid;}
	
	~Array_2D();
};


/********MEMBER FUNCTIONS********/
template <class T>
Array_2D<T>::Array_2D(unsigned width, unsigned height, T default_value){
	xgrid = width;
	ygrid = height;
	unsigned s = width * height;
	values = new T[ s ];
	for (unsigned i = 0; i < s; ++i)
		values[i] = default_value;
}

template <class T>
Array_2D<T>::Array_2D(const Array_2D<T>& rhs){
	unsigned i, s = rhs.xgrid * rhs.ygrid;
	values = new T[s];
	
	for (i = 0; i < s; ++i){
		values[i] = rhs.values[i];
	}

	xgrid = rhs.xgrid;
	ygrid = rhs.ygrid;
}

template <class T>
T& Array_2D<T>::operator()(int x, int y){
	if (x < 0 || x > (int)xgrid || y < 0 || y > (int)ygrid)
		return values[0];
	
	return (values[x + y * xgrid]);
}

template <class T>
const T& Array_2D<T>::operator()(int x, int y) const{
	if (x < 0 || x > (int)xgrid || y < 0 || y > (int)ygrid)
		return values[0];
	
	return (values[x + y * xgrid]);
}

template <class T>
Array_2D<T>& Array_2D<T>::operator=( const Array_2D<T> &rhs ){
	//check for self-assignment
	if (this != &rhs){
		unsigned i, s = rhs.xgrid * rhs.ygrid;
		delete[] values;
		values = new T[s];
		
		for (i = 0; i < s; ++i){
			values[i] = rhs.values[i];
		}

		xgrid = rhs.xgrid;
		ygrid = rhs.ygrid;
	}

	return (*this);
}

template <class T>
Array_2D<T>& Array_2D<T>::operator+=( const Array_2D<T>& rhs ){
	unsigned i,j;
	for (i = 0; i < xgrid && i < rhs.xgrid; ++i){
		for (j = 0; j < ygrid && j < rhs.ygrid; ++j){	
			values[i + j * xgrid] += rhs.values[i + j * rhs.xgrid];
		}
	}
	
	return (*this);
}

template <class T>
Array_2D<T>& Array_2D<T>::operator-=( const Array_2D<T>& rhs ){
	unsigned i,j;
	for (i = 0; i < xgrid && i < rhs.xgrid; ++i){
		for (j = 0; j < ygrid && j < rhs.ygrid; ++j){	
			values[i + j * xgrid] -= rhs.values[i + j * rhs.xgrid];
		}
	}
	
	return (*this);
}

template <class T>
Array_2D<T>::~Array_2D(){
	delete[] values;
}

#endif
