#ifndef SIMPLEXNOISE_H
#define SIMPLEXNOISE_H

#include "Array2D.h"

class SimplexNoise{
	unsigned 	xgrid,//size variables, width and height
				ygrid;
				
	unsigned 	permutation_table[256];//randomized tables
	float 		gradient_table[8][3];
	
	unsigned 	max_octaves; //Fractional Brownian Motion variables
	float		lacunarity;
	float		gain;
	float		start_frequency;
	float		start_amplitude;
	
	float make_point(float x, float y, float z);
	float radial_attenuation(float* distance_vector, int gradient_index);
	int myfloor(float value);
	
public:
	SimplexNoise(unsigned Octaves, float Gain, float Lacunarity, float Frequency = 0.002f, float Amplitude = 50.0f);
	void makeSomeNoise(Array_2D<float>& height_map, int z);
};
#endif
