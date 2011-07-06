#include "colorizer.h"
#include "color.h"
#include "Array2D.h"
#include <map>
#include <string>
#include <cfloat>
#include <iostream>
#include <cstdlib>

using namespace std;

void colorize(const Array_2D<float>& height_map, color_map& ret_val, gradient& Gradient){
	//next, find the min and max
	unsigned i,j;
	float min = FLT_MAX,
		  max = -FLT_MAX;
	for (i=0; i<ret_val.width(); ++i){
		for (j=0; j<ret_val.height(); ++j){
			if (height_map(i,j) < min) min = height_map(i,j);
			if (height_map(i,j) > max) max = height_map(i,j);
		}
	}
	float diff = max - min;
	
	//Now, for each point make a color.
	float adjusted_height, rung_min, rung_max;
	gradient::iterator p;
	color low, high;
	
	for (i=0; i < ret_val.width(); ++i){
		for (j=0; j < ret_val.height(); ++j){
			//First, get the adjusted height between 0.0 and 1.0, with min=0 and max=1.
			adjusted_height = (height_map(i,j) - min ) / diff;
			
			//get the surrounding colors in the gradient
			//	first, get the upper rung
			p = Gradient.upper_bound(adjusted_height);
			
			if (p == Gradient.end()){//check for a boundary case (rare but possible)
				ret_val(i,j) = Gradient[1.0f];
			}
			else{
				high = p->second;
				rung_max = p->first;
				
				//	Next, get the lower rung.
				--p;
				low = p->second;
				rung_min = p->first;
				
				//	Readjust it to the rungs.
				adjusted_height = (adjusted_height - rung_min) / (rung_max - rung_min);
				
				//	Finally, make the final color.
				ret_val(i,j) = lerp(low, high, adjusted_height);
			}
		}
	}
}

//A helper function that fills in a <float,color> map (a gradient)
//	with the value pairs specified in the string 'post'. 'post' is
//	probably a command-line argument, but could just be used to save space.
void make_gradient(gradient& grad, string post){
	color parsed_color;
	float parsed_float;
	string color_string, float_string;
	bool found_paren;
	
	//1.0 Pull out pairs (separated by commas) and put them in the map.
	for (unsigned i = 0; i < post.length(); ++i){ //loop through every character...

		//1.1 Get the float string. 
		//	Note: the pair is separated internally by an equal sign; 
		//	pairs are separated from each other by colons.
		for (float_string = ""; post[i] != '=' && i < post.length(); ++i)
			float_string += post[i];
		
		//1.2 Get the color string.
		color_string = "";
		found_paren = false;
		for (++i; post[i] != ':' && i < post.length();++i)
			color_string += post[i];
		
		//1.3 Check for a premature ending.
		if (float_string == "" || color_string == ""){
			cout << "Warning: Premature end of gradient string." << endl; //If the lower and/or upper bound is missing, they/it will be filled in below in step 2.0.
			break;
		}
		
		//1.4 If the string didn't end prematurely, parse the two substrings.
		parsed_float = atof(float_string.c_str());
		parsed_color = color(color_string); //The constructor will fire off an error message and return a default value, if necessary.
		
		//1.5 Finally, input the two into the gradient map
		grad[parsed_float] = parsed_color;
	}
	
	//2.0 make sure the values [0.0f] and [1.0f] are included
	//	2.1 [0.0f]
	if (grad.find(0.0f) == grad.end()){
		grad[0.0f] = color("white");
		cout << "Warning: A lower bound (0.0) is required for all gradients. White has been inserted" << endl;
	}
	
	//	2.2 [1.0f]
	if (grad.find(1.0f) == grad.end()){
		grad[1.0f] = color("black");
		cout << "Warning: An upper bound (1.0) is required for all gradients. Black has been inserted" << endl;
	}
}

//Helper function for use in debugging
void print_gradient(gradient& grad){
	float f;
	color c;
	for (gradient::iterator i = grad.begin(); i != grad.end(); ++i){
		f = i->first;
		c = i->second;
		
		cout << f << ": " << int(c.v[0]) << ", " << int(c.v[1]) << ", " << int(c.v[2]) << endl;
	}
}
