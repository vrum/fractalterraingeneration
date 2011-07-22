#ifndef COLOR_H
#define COLOR_H

#include "Array2D.h"
#include <string>

typedef unsigned char byte;

struct color{
	//this union means the color is stored in 4 bytes, but is accessible in 3 ways
	union{
		//NOTE: depending on machine specifics, 'fullval' can return
		//	unpredictable results. It should only be used for easier
		//	inter-color communication.
		unsigned fullval;
		byte v[4]; 
		struct { byte r, g, b, a; };
	};
	
	color(byte R, byte G, byte B, byte A) : r(R),g(G),b(B),a(A) { }
	color(byte R, byte G, byte B) : r(R),g(G),b(B),a(255) { }
	color(unsigned Fullval):fullval(Fullval) { }
	color():fullval(0u) { }//transparent black
	color(const color& rhs){fullval = rhs.fullval;}
	color(const std::string& cname);
	
	color& operator=(const color& rhs);
	color& operator+=(const color& rhs); //note: this is the OVER operation
	color& operator-=(const color& rhs);
	const color operator+(const color& rhs) const {return (color(*this)+=rhs);}
	const color operator-(const color& rhs) const {return (color(*this)-=rhs);}
	bool operator==(const color& rhs) const {return fullval == rhs.fullval;}
	bool operator!=(const color& rhs) const {return !(*this == rhs);}

	//lesser used functions
	color& value_addition(const color& rhs); //ignores the alpha channels
	color& value_subtraction(const color& rhs);// ignores the alpha channels
};

color lerp(color c1, color c2, float value);

class color_map : public Array_2D<color>{
public:
	color_map(unsigned Xgrid, unsigned Ygrid):Array_2D<color>(Xgrid,Ygrid) {}

	void print(std::string filename) const;
	color_map& value_addition( const color_map& rhs );
	color_map& value_subtraction( const color_map& rhs );
};

#endif
