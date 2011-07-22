#include "color.h"
#include "Array2D.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

/*		GLOBAL FUNCTION		*/
color lerp(color c1, color c2, float value){
	color tcolor(0,0,0);
	
	if (value <= 0.0f)
		return (c1);
	else if (value >= 1.0f)
		return (c2);
	
	for (int g=0;g<4;g++){
		if (c1.v[g] == c2.v[g])
			tcolor.v[g] = c1.v[g];
		else
			tcolor.v[g] = byte(float(c1.v[g]) * (1.0f - value) + float(c2.v[g]) * value);
	}
	
	return (tcolor);
}

/*		COLOR MEMBER FUNCTIONS		*/
color::color(const string& cname){
	//error checking
	if (cname == "")
	{
		cout << "Warning: No color specified!"<<endl;
		fullval=0;
		return;
	}
	
	//there can be 1 number, or 3 numbers separated by commas 
	if (cname.find(',') == string::npos)
	{
		//one number
		r = g = b = (byte)(atoi(cname.c_str()));
	}
	else
	{
		//three numbers
		byte tchar = 0;
		int pos = 0;
		unsigned i;
		for (i=0; i < cname.size(); ++i)
		{
			if (cname[i] == ',')
			{
				v[pos] = tchar;
				++pos;
				tchar = 0;
			}
			else
			{
				tchar *= 10;
				tchar += (byte)(cname[i]-'0');
			}
		}
		
		g = tchar;
	}

	a = 255;
}

color& color::operator=(const color& rhs){
	fullval=rhs.fullval;
	return (*this);
}

color& color::operator+=(const color& rhs){
	//mix the alpha channels. NOTE: the right operand is considered to be the top layer.
	float f1 = (float)a / 255.0f,
		  f2 = (float)rhs.a / 255.0f,
		  f3 = f1 + (1.0f-f1) * f2; //resulting alpha in the 0.0-1.0 range

	a = byte(f3 * 255.0f);
	
	//mix the three color components
	f1 = ((float)rhs.r * (float)rhs.a / 255.0f + (float)r * (1.0f - (float)rhs.a / 255.0f) * (float)a / 255.0f) / f3;
	f2 = ((float)rhs.g * (float)rhs.a / 255.0f + (float)g * (1.0f - (float)rhs.a / 255.0f) * (float)a / 255.0f) / f3;
	f3 = ((float)rhs.b * (float)rhs.a / 255.0f + (float)b * (1.0f - (float)rhs.a / 255.0f) * (float)a / 255.0f) / f3;
	
	r = (f1 > 255.0f) ? 255 : (byte)f1;
	g = (f2 > 255.0f) ? 255 : (byte)f2;
	b = (f3 > 255.0f) ? 255 : (byte)f3;
	
	return (*this);
}

color& color::operator-=(const color& rhs){
	//Mix the three color components. NOTE: the right operand is considered to be the top layer.
	float f1 = (float)r * (1.0f - (float)rhs.a / 255.0f) * (float)a / 255.0f - (float)rhs.r * (float)rhs.a / 255.0f,
		  f2 = (float)g * (1.0f - (float)rhs.a / 255.0f) * (float)a / 255.0f - (float)rhs.g * (float)rhs.a / 255.0f,
		  f3 = (float)b * (1.0f - (float)rhs.a / 255.0f) * (float)a / 255.0f - (float)rhs.b * (float)rhs.a / 255.0f;
	
	r = (f1 < 0.0f) ? 0 : (byte)f1;
	g = (f2 < 0.0f) ? 0 : (byte)f2;
	b = (f3 < 0.0f) ? 0 : (byte)f3;
	
	return (*this);
}

color& color::value_addition(const color& rhs){ //ignores the alpha values
	int ir = (int)r + (int)rhs.r,
		ig = (int)g + (int)rhs.g,
		ib = (int)b + (int)rhs.b;
	//alpha channel remains unchanged

	r = (ir > 255) ? 255 : byte(ir);
	g = (ig > 255) ? 255 : byte(ig);
	b = (ib > 255) ? 255 : byte(ib);

	return (*this);
}

color& color::value_subtraction(const color& rhs){ //ignores the alpha values
	int ir = (int)r - (int)rhs.r,
		ig = (int)g - (int)rhs.g,
		ib = (int)b - (int)rhs.b;
	//alpha channel remains unchanged

	r = (ir < 0) ? 0 : byte(ir);
	g = (ig < 0) ? 0 : byte(ig);
	b = (ib < 0) ? 0 : byte(ib);

	return (*this);
}



/*		COLOR_MAP MEMBER FUNCTIONS		*/
void color_map::print(string filename) const{
//set up some variables
	int i;
	unsigned j,k;

	//1.0 open output file
	ofstream out;
	out.open(filename.c_str(), ofstream::binary);
	if (!(out.is_open())){
		cout << "Target file opening error.\nEither access is denied, or the file is locked by another program."<<endl;
		exit(0);
	}
	
	//2.0 copy the header
	//2.1 magic number
	out.put(char(66));
	out.put(char(77));
	
	//2.2 filsize/unused space
	for (i=0;i<8;i++)
		out.put(char(0));
	
	//2.3 data offset
	out.put(char(54));
	
	//2.4 unused space
	for (i=0;i<3;i++)
		out.put(char(0));
	
	//2.5 header size
	out.put(char(40));
	
	//2.6 unused space
	for (i=0;i<3;i++)
		out.put(char(0));
	
	//2.7 file width (trickier)
	out.put(char(xgrid%256));
	out.put(char((xgrid>>8)%256));
	out.put(char((xgrid>>16)%256));
	out.put(char((xgrid>>24)%256));
	
	//2.8 file height (trickier)
	out.put(char(ygrid%256));
	out.put(char((ygrid>>8)%256));
	out.put(char((ygrid>>16)%256));
	out.put(char((ygrid>>24)%256));
	
	//2.9 color planes
	out.put(char(1));
	out.put(char(0));
	
	//2.10 bit depth
	out.put(char(24));
	
	//2.11 the rest
	for (i=0;i<25;i++)
		out.put(char(0));
	
	//3.0 put in the elements of the array
	for (i=(ygrid-1);i>=0;i--){//bitmaps start with the bottom row, and work their way up...
		for (j=0;j<xgrid;j++){//...but still go left to right
			if (values[j + i * xgrid].v[3] < 255){//if there is incomplete alpha...
				color tc = color(0,0,0,255) + values[j + i * xgrid];
				
				out.put(char(tc.v[2]));
				out.put(char(tc.v[1]));
				out.put(char(tc.v[0]));
			}
			else{
				//Print out the color (in reverse).
				out.put(char(values[j + i * xgrid].v[2]));//blue
				out.put(char(values[j + i * xgrid].v[1]));//green
				out.put(char(values[j + i * xgrid].v[0]));//red
			}
		}
		
		//round off the row
		for (k=0;k<(xgrid%4);k++)
			out.put(char(0));
	}
	
	//4.0 end the file
	out.close();
}


color_map& color_map::value_addition( const color_map& rhs ){
	//For every pixel, mix them together. Note: if the two maps have 
	//	different dimensions, as much of rhs as possible is copied.
	unsigned i,j;
	for (i = 0; i < xgrid && i < rhs.xgrid; ++i){
		for (j = 0; j < ygrid && j < rhs.ygrid; ++j){	
			operator()(i,j).value_addition(rhs(i,j));
		}
	}
	
	return (*this);
}


color_map& color_map::value_subtraction( const color_map& rhs ){
	//For every pixel, mix them together. Note: if the two maps have 
	//	different dimensions, as much of rhs as possible is copied.
	unsigned i,j;
	for (i = 0; i < xgrid && i < rhs.xgrid; ++i){
		for (j = 0; j < ygrid && j < rhs.ygrid; ++j){	
			operator()(i,j).value_subtraction(rhs(i,j));
		}
	}
	
	return (*this);
}
