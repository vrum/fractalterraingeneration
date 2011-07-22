#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include "CellNoise.h"
#include "color.h"
#include "Array2D.h"

using namespace std;

string secondsToString(int sec);

int main(int argc, char* argv[])
{
	//first, set up some variables
	unsigned i;
	time_t beginning;
	
	//get the command line arguments
	unsigned xgrid = 500;
	unsigned ygrid = 200;
	unsigned zgrid = 500;
	unsigned point = 500;
	unsigned buksz = 100;
	unsigned rseed = unsigned(time(NULL));
	bool wrapping  = true;
	bool clipping  = false;
	string fchunk = "part";
	string disfunc = "euclideanSquared";
	float minkowski = -10000.0f;
	
	for (i = 1; i < (unsigned)argc; ++i)
	{
		//first, find the position of the '=' in the command
		string arg(argv[i]);
		size_t pos = arg.find('=');
		
		if (pos != string::npos)
		{
			//get the prescript and the postscript of the argument
			string pre = arg.substr(1,pos-1),
				   post= arg.substr(pos+1);

			if (pre == "dis" || pre == "distance")
			{
				disfunc = post;
			}
			else if (pre == "xgrid" || pre == "x" || pre=="width")
			{
				xgrid = atoi(post.c_str());
			}
			else if (pre == "ygrid" || pre == "y" || pre=="height")
			{
				ygrid = atoi(post.c_str());
			}
			else if (pre == "zgrid" || pre == "z" || pre == "frames")
			{
				zgrid = atoi(post.c_str());
			}
			else if (pre == "bucketSize" || pre=="bucket_size" || pre == "bucketsize" || pre == "bucket")
			{
				buksz = atoi(post.c_str());
			}
			else if (pre == "num_points" || pre == "points" || pre == "numPoints" || pre == "number_of_points")
			{
				point = atoi(post.c_str());
			}
			else if (pre == "wrapping" || pre == "wrap")
			{
				if (post == "false" || post == "no")
					wrapping = false;
			}
			else if (pre == "clipping" || pre == "clamping" || pre == "clip" || pre == "clamp")
			{
				if (post == "true" || post == "yes")
					clipping = true;
			}
			else if (pre == "filename" || pre == "fname" || pre == "file")
			{
				fchunk = post;
			}
			else if (pre == "seed")
			{
				rseed = atoi(post.c_str());
			}
			else if (pre == "minkowski" || pre == "Minkowski")
			{
				minkowski = atof(post.c_str());
			}
			else
			{
				cout << "Command \'" << pre << "\' not recognized. Please reference the manual." << endl; 
			}
		}
	}
	
	//set the random seed
	srand(rseed);
	
	//make the cell noise object
	CellNoise noise(xgrid,ygrid,zgrid,point,buksz,wrapping);
	noise.setDistanceFunction(disfunc);
	if (minkowski != -10000.0f) noise.setMinkowski(minkowski);
	
	//make the color map
	color_map colmap(xgrid,ygrid);
	
	//get started...
	beginning = time(NULL);
	cout << "Starting up..." << endl;
	
	string fname = fchunk + "000.bmp";
	int len = fchunk.length();
	
	//loop!
	for (i = 0; i < zgrid; ++i)
	{
		//first, make the filename
		//	there are many (probably easier) ways to do this, but this is efficient and simple
		++fname[len+2];//characters have the useful ability to exceed 9, meaning it's very easy to roll over
		if (fname[len+2] > '9'){++fname[len+1];fname[len+2]='0';}
		if (fname[len+1] > '9'){++fname[len];  fname[len+1]='0';}//this won't work if you exceed 999!

		//now make noise!
		noise.makeSomeNoise(colmap,i);
		
		//print out the color map
		colmap.print(fname);
		
		//finally, update the user (occasionally)
		if ((i & 15) == 15){
			cout << i << " frames done. Estimated time remaining: "
				 << secondsToString(int(double(time(NULL)-beginning) * double(zgrid - i) / (double)i))
				 << ". " << endl;
		}
	}
	
	//finally, spit out the total time
	cout << "\nDone. Total time taken: " << secondsToString(int(double(time(NULL) - beginning))) << " for " << i << " frames.\n"
		 << "Average time per frame: " << (double(time(NULL))-double(beginning))/double(i) << " seconds.\n" 
		 << "The seed was "<< rseed << '.' << endl;
}

string secondsToString(int sec)
{
	int hours = sec / 3600,
		minutes = (sec % 3600) / 60,
		seconds = sec % 60;
		
	stringstream s;
	
	if (hours)
		s << hours << " hours, ";
	if (minutes)
		s << minutes << " minutes, and ";
	s << seconds << " seconds";
	
	return (s.str());
}
