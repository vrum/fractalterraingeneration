#include "CellNoise.h"
#include <cfloat>
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

void CellNoise::construct(unsigned Xgrid, unsigned Ygrid, unsigned Zgrid, unsigned Num_points, unsigned Bucket_size, bool Wrapping)
{
	unsigned int i,j,x,y,z;
	float l,m,n;
	location loc;

	xgrid = Xgrid;
	ygrid = Ygrid;
	zgrid = Zgrid;
	
	num_points = Num_points;
	bucket_size = Bucket_size;
	
	wrapping = Wrapping;
	
	buckets_x = (xgrid / bucket_size) + 2;
	buckets_y = (ygrid / bucket_size) + 2;
	buckets_z = (zgrid / bucket_size) + 2;
	
	distanceFunction = &CellNoise::euclideanSquared;
	minkowski_coefficient = 4.0f;
	
	buckets = new vector<location>**[buckets_x];
	for (i = 0; i < buckets_x; ++i)
	{
		buckets[i] = new vector<location>*[buckets_y];
		
		for (j = 0; j < buckets_y; ++j)
		{
			buckets[i][j] = new vector<location>[buckets_z];
		}
	}
	
	//fill up the buckets
	for(i = 0; i < Num_points; ++i)
	{
		x = rand() % buckets_x;
		y = rand() % buckets_y;
		z = rand() % buckets_z;
		
		//make a new entry
		//	make the new coordinates
		l = float(rand()&1023)/1023.0f;
		m = float(rand()&1023)/1023.0f;
		n = float(rand()&1023)/1023.0f;
		
		loc.x = l + float(x);
		loc.y = m + float(y);
		loc.z = n + float(z);

		int randle = rand();

		loc.col = color(byte(randle & 255), byte((randle >> 8) & 255), byte((randle >> 16) & 255) );
		
		buckets[x][y][z].push_back(loc);
	}
	
	//if wrapping, wrap z-wise
	if (wrapping)
	{
		for (x = 0; x < buckets_x; ++x)
		{
			for (y = 0; y < buckets_y; ++y)
			{
				buckets[x][y][buckets_z-1].clear();
				buckets[x][y][0].clear();
				
				for (i = 0; i < buckets[x][y][1].size(); ++i)
				{
					loc = buckets[x][y][1][i];
					loc.z += float(buckets_z - 2);
					buckets[x][y][buckets_z - 1].push_back(loc);
				}
				
				for (i = 0; i < buckets[x][y][buckets_z-2].size(); ++i)
				{
					loc = buckets[x][y][buckets_z-2][i];
					loc.z -= float(int(loc.z));//basically fmod 1, since it's going in the 0'th bucket
					buckets[x][y][0].push_back(loc);
				}
			}
		}
	}
}
	
CellNoise::CellNoise()
{
	construct(500,200,500,500,100,true);
}

CellNoise::CellNoise(unsigned Xgrid, unsigned Ygrid, unsigned Zgrid, unsigned Num_points, unsigned Bucket_size, bool Wrapping)
{
	construct(Xgrid,Ygrid,Zgrid,Num_points,Bucket_size,Wrapping);
}

CellNoise::~CellNoise()
{
	for (unsigned i = 0; i < buckets_x; ++i)
	{
		for (unsigned j = 0; j < buckets_y; ++j)
		{
			delete[] buckets[i][j];
		}
		delete[] buckets[i];
	}
	
	delete[] buckets;
}

void CellNoise::makeSomeNoise(color_map& image, int zz)
{
	//set up some variables
	int	i,j,k,//iterators
		x,y,//location variables
		bx,by,bz,//bucket indexes
		mx = xgrid + bucket_size,
		my = ygrid + bucket_size,
		z = zz + bucket_size;
		
	unsigned p;
	location loc;
	
	float 	temp_dis,
			fx,fy,fz,
			fClosest;
	color cClosest;

	//get started
	for (x = bucket_size; x < mx; ++x)
	{
		for (y = bucket_size; y < my; ++y)
		{
			//get the immediate bucket
			fx = (float)x / (float)bucket_size;
			fy = (float)y / (float)bucket_size;
			fz = (float)z / (float)bucket_size;
			
			bx = int(fx);
			by = int(fy);
			bz = int(fz);
			
			//clear out the old distance
			fClosest = FLT_MAX;
			cClosest = color(0,0,0);
			
			//check each of the points in the surrounding buckets
			for (i = -1; i < 2; ++i)
			{
				for (j = -1; j < 2; ++j)
				{
					for (k = -1; k < 2; ++k)
					{
						for (p = 0; p < buckets[bx+i][by+j][bz+k].size(); ++p)
						{
							//find the distance to this point
							loc = buckets[bx+i][by+j][bz+k][p];
							temp_dis = (this->*distanceFunction)(fx, fy, fz, loc.x, loc.y, loc.z);

							//if this is the closest so far...
							if (temp_dis < fClosest){
								fClosest = temp_dis;
								cClosest = loc.col;
							}
						}
					}
				}
			}
			
			//now that we have the value, put it in
			image(x - bucket_size, y - bucket_size) = cClosest;
		}
	}
}

void CellNoise::setDistanceFunction(string dis)
{
	if (dis == "euclidean")
		distanceFunction = &CellNoise::euclidean;
	else if (dis == "euclideanSquared")
		distanceFunction = &CellNoise::euclideanSquared;
	else if (dis == "manhattan")
		distanceFunction = &CellNoise::manhattan;
	else if (dis == "chebychev")
		distanceFunction = &CellNoise::chebychev;
	else if (dis == "quadratic")
		distanceFunction = &CellNoise::quadratic;
	else if (dis == "minkowski")
		distanceFunction = &CellNoise::minkowski;
	else
		cout << "Warning: distance function \'" << dis << "\' not recognized." << endl;
}

void CellNoise::setMinkowski(float coefficient)
{
	distanceFunction = &CellNoise::minkowski;
	minkowski_coefficient = coefficient;
}

int CellNoise::getXgrid(){return xgrid;}
int CellNoise::getYgrid(){return ygrid;}
int CellNoise::getZgrid(){return zgrid;}

float CellNoise::euclidean(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float dif_x = x1 - x2,
		  dif_y = y1 - y2,
		  dif_z = z1 - z2;
	
	return (pow(dif_x * dif_x + dif_y * dif_y + dif_z * dif_z,0.5f));
}

float CellNoise::euclideanSquared(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float	dif_x = x1-x2,
			dif_y = y1-y2,
			dif_z = z1-z2;

	return (dif_x * dif_x + dif_y * dif_y + dif_z * dif_z);
}

float CellNoise::manhattan(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float	dif_x = fabs(x1 - x2),
			dif_y = fabs(y1 - y2),
			dif_z = fabs(z1 - z2);

	return (dif_x + dif_y + dif_z);
}

float CellNoise::chebychev(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float	dif_x = fabs(x1 - x2),
			dif_y = fabs(y1 - y2),
			dif_z = fabs(z1 - z2);

	if (dif_x > dif_y && dif_x > dif_z)
		return (dif_x);
	else if (dif_y > dif_x && dif_y > dif_z)
		return (dif_y);
	else
		return (dif_z);
}

float CellNoise::quadratic(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float	dif_x = x1 - x2,
			dif_y = y1 - y2,
			dif_z = z1 - z2;
			
	return (dif_x * dif_x + dif_y * dif_y + dif_z * dif_z + dif_x * dif_y + dif_x * dif_z + dif_y * dif_z);
}

float CellNoise::minkowski(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float	ddx = pow(fabs(x1 - x2), minkowski_coefficient),
			ddy = pow(fabs(y1 - y2), minkowski_coefficient),
			ddz = pow(fabs(z1 - z2), minkowski_coefficient);
	
	return (pow(ddx + ddy + ddz,1.0f/minkowski_coefficient));
}
