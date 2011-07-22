#ifndef CELLNOISE_H
#define CELLNOISE_H
#include <vector>
#include <string>
#include "Array2D.h"
#include "color.h"

struct location{
	color col;
	float x,y,z;
	location(float X, float Y, float Z, color Col): x(X), y(Y), z(Z), col(Col) { }
	location() { x = y = z = 0.0f; }
};

class CellNoise
{
	bool wrapping;
	
	unsigned xgrid,ygrid,zgrid;
	unsigned num_points;
	
	std::vector<location> ***buckets;
	
	int bucket_size;
	unsigned buckets_x,buckets_y,buckets_z;
	
	float (CellNoise::*distanceFunction)(float x1, float y1, float z1, float x2, float y2, float z2);
	float minkowski_coefficient;
	
	float euclidean(float x1, float y1, float z1, float x2, float y2, float z2);
	float euclideanSquared(float x1, float y1, float z1, float x2, float y2, float z2);
	float manhattan(float x1, float y1, float z1, float x2, float y2, float z2);
	float chebychev(float x1, float y1, float z1, float x2, float y2, float z2);
	float quadratic(float x1, float y1, float z1, float x2, float y2, float z2);
	float minkowski(float x1, float y1, float z1, float x2, float y2, float z2);
	
	void construct(unsigned Xgrid, unsigned Ygrid, unsigned Zgrid, unsigned Num_points, unsigned Bucket_size, bool Wrapping);
	
public:
	CellNoise();
	CellNoise(unsigned Xgrid, unsigned Ygrid, unsigned Zgrid, unsigned Num_points, unsigned Bucket_size, bool Wrapping);
	~CellNoise();
	
	void makeSomeNoise(color_map& image, int zz);
	
	//getters/setters
	void setDistanceFunction(std::string dis);
	void setMinkowski(float coefficient);
	
	int getXgrid();
	int getYgrid();
	int getZgrid();
};
#endif
