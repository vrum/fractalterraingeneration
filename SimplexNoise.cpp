#include "SimplexNoise.h"
#include "Array2D.h"
#include <cmath>
#include <cstdlib>
#include <iostream>//DEBUG

using namespace std;

SimplexNoise::SimplexNoise(unsigned Octaves, float Gain, float Lacunarity, float Frequency, float Amplitude){
	max_octaves=Octaves;
	gain=Gain;
	lacunarity=Lacunarity;
	start_amplitude = Amplitude;
	start_frequency = Frequency;
	
	//set up the permutation table
	unsigned i,j,k;
	for (i=0;i<256;++i)
		permutation_table[i]=i;
	
	for (i=0;i<256;++i){
		j = rand() & 255;
		k=permutation_table[i];
		permutation_table[i] = permutation_table[j];
		permutation_table[j] = k;
	}
	
	//set up the gradient table
	for (i=0;i<8;++i){
		for (j=0,k=1;j<3;++j,k<<=1){
			gradient_table[i][j] = (i & k) ? -1 : 1;
		}
	}
}

void SimplexNoise::makeSomeNoise(Array_2D<float>& height_map, int z){
	unsigned x,y,octave;
	float amplitude, frequency, final_val;
	
	//for each pixel...
	for (x=0;x<height_map.width();++x){
		for (y=0;y<height_map.height();++y){
			frequency = start_frequency;
			amplitude = start_amplitude;
			final_val = 0.0f;
			
			//for each octave...
			for (octave = 0; octave < max_octaves; ++octave){
				final_val += amplitude * make_point((float)x * frequency, (float)y * frequency, (float)z * frequency);
				
				frequency *= lacunarity;
				amplitude *= gain;
			}
			
			height_map(x,y) = final_val;
		}
	}

	//normalize the height_map
	//	first, find the min
	float min = height_map(0,0);
	for (x = 0; x < height_map.width(); ++x){
		for (y = 0; y < height_map.height(); ++y){
			if (height_map(x,y) < min) min = height_map(x,y);
		}
	}
	
	//	next, add the min to everything so the new minimum is 0.0
	for (x = 0; x < height_map.width(); ++x){
		for (y = 0; y < height_map.height(); ++y)
			height_map(x,y) += min;
	}
}

float SimplexNoise::make_point(float x, float y, float z){
	int corners[4][3];//4 corners, each with an x,y, and z coordinate. Note: only corners[0] contains original values; the other three are offset values from corners[0].
	float distances[4][3];//the distances to each of the four corners
	int i,j;
	
	//first, get the bottom corner in skewed space
	float general_skew = 1.0f / 3.0f; //very nice general skew/unskew values in 3D
	float specific_skew = (x + y + z) * general_skew;
	corners[0][0] = myfloor(x + specific_skew);
	corners[0][1] = myfloor(y + specific_skew);
	corners[0][2] = myfloor(z + specific_skew);
	
	//next, get the distance vectors to the bottom corner
	float general_unskew = 1.0f / 6.0f;
	float specific_unskew = float(corners[0][0] + corners[0][1] + corners[0][2]) * general_unskew;
	distances[0][0] = x - float(corners[0][0]) + specific_unskew;
	distances[0][1] = y - float(corners[0][1]) + specific_unskew;
	distances[0][2] = z - float(corners[0][2]) + specific_unskew;
	
	//find the coordinates for the two middle corners
	if (distances[0][0] < distances[0][1]){ // y > x
		if (distances[0][1] < distances[0][2]){ // if z > y > x
			corners[1][0] = 0;
			corners[1][1] = 0;
			corners[1][2] = 1;
			
			corners[2][0] = 0;
			corners[2][1] = 1;
			corners[2][2] = 1;
		}
		else if (distances[0][0] < distances[0][2]){ // if y > z > x
			corners[1][0] = 0;
			corners[1][1] = 1;
			corners[1][2] = 0;
			
			corners[2][0] = 0;
			corners[2][1] = 1;
			corners[2][2] = 1;
		}
		else{ // y > x > z
			corners[1][0] = 0;
			corners[1][1] = 1;
			corners[1][2] = 0;
			
			corners[2][0] = 1;
			corners[2][1] = 1;
			corners[2][2] = 0;
		}
	}
	else{ // x > y
		if (distances[0][0] < distances[0][2]){ // z > x > y
			corners[1][0] = 0;
			corners[1][1] = 0;
			corners[1][2] = 1;
			
			corners[2][0] = 1;
			corners[2][1] = 0;
			corners[2][2] = 1;
		}
		else if (distances[0][1] < distances[0][2]){ // x > z > y
			corners[1][0] = 1;
			corners[1][1] = 0;
			corners[1][2] = 0;
			
			corners[2][0] = 1;
			corners[2][1] = 0;
			corners[2][2] = 1;
		}
		else{ //x > y > z
			corners[1][0] = 1;
			corners[1][1] = 0;
			corners[1][2] = 0;
			
			corners[2][0] = 1;
			corners[2][1] = 1;
			corners[2][2] = 0;
		}
	}
	
	//get the top corner
	corners[3][0]=1;
	corners[3][1]=1;
	corners[3][2]=1;
	
	//get the distances
	for (i=1;i<=3;++i){
		for (j=0;j<3;++j){
			distances[i][j] = distances[0][j] - float(corners[i][j]) + general_unskew * (float)i;
		}
	}
	
	//get the gradients indices
	int gradient_index[4];
	
	gradient_index[0] = permutation_table[(corners[0][0] + permutation_table[(corners[0][1] + permutation_table[corners[0][2] & 255]) & 255]) & 255] & 7;
	for (i=1;i<4;++i)
		gradient_index[i] = permutation_table[(corners[0][0] + corners[i][0] + permutation_table[(corners[0][1] + corners[i][1] + permutation_table[(corners[0][2] + corners[i][2]) & 255]) & 255]) & 255] & 7;

	//sum the contributions from each corner, found using radial attenuation
	float final_sum = 0.0f;
	for (i=0;i<4;++i){
		final_sum += radial_attenuation(distances[i], gradient_index[i]);
	}
	
	return (32.0f * final_sum);
}

float SimplexNoise::radial_attenuation(float* distances, int gradient_index){
	float test_product = 0.6f - distances[0] * distances[0] - distances[1] * distances[1] - distances[2] * distances[2];
	
	if (test_product < 0.0f)
		return (0.0f);
		
	float dot_product = distances[0] * gradient_table[gradient_index][0] + distances[1] * gradient_table[gradient_index][1] + distances[2] * gradient_table[gradient_index][2];
		
	test_product *= test_product; //square it
	
	return (test_product * test_product * dot_product);
}

int SimplexNoise::myfloor(float value)
{
	return (value >= 0 ? (int)value : (int)value-1);
}
