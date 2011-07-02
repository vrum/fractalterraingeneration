#include "color.h"
#include "colorizer.h"
#include "SimplexNoise.h"
#include "shader.h"
#include "Array2D.h"
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <map>
#include <fstream>

using namespace std;

string secondsToString(int sec);

int main(int argc, char* argv[])
{
	//first, set up some variables
	unsigned i;
	time_t beginning;
	
	//colors
	color waterlow(0,0,55),
		  waterhigh(0,53,106),
		  landlow(0,64,0),
		  landhigh(133,182,116),
		  mountlow(167,157,147),
		  mounthigh(216,223,226);
	
	//get the command line arguments
	unsigned xgrid = 500;
	unsigned ygrid = 200;
	unsigned zgrid = 500;
	unsigned octaves = 16;
	unsigned rseed = unsigned(time(NULL));
	string fchunk = "part";
	float lacunarity = 2.0f;
	float gain = 0.65f;
	gradient grad; grad[0.0f] = waterlow; grad[0.5] = waterhigh; grad[0.501]=landlow; grad[0.85]=landhigh; grad[0.8501]=mountlow; grad[1.0f] = mounthigh;
	byte shade=70,
		 highlight=70;
	int kernel = 3;
	float phi = 0.8f;
	
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
				   
			//cout << endl <<"Pre: \'" << pre << "\' Post: \'"<< post << '\'' << endl; // for easy debugging
			
			if (pre == "xgrid" || pre == "x" || pre=="width")
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
			else if (pre == "filename" || pre == "fname" || pre == "file")
			{
				fchunk = post;
			}
			else if (pre == "seed")
			{
				rseed = atoi(post.c_str());
			}
			else if (pre == "gradient" || pre == "grad" || pre=="colors")
			{
				make_gradient(grad, post);
			}
			else if (pre == "octaves" || pre == "octave")
			{
				octaves = atoi(post.c_str());
			}
			else if (pre == "gain")
			{
				gain = atoi(post.c_str());
			}
			else if (pre == "lacunarity" || pre == "lac")
			{
				lacunarity = atof(post.c_str());
			}
			else if (pre == "shade" || pre=="shadow")
			{
				shade = (byte)atoi(post.c_str());
			}
			else if (pre == "highlight")
			{
				highlight = (byte)atoi(post.c_str());
			}
			else if (pre == "kernel")
			{
				kernel = atoi(post.c_str());
			}
			else if (pre == "phi")
			{
				phi = atof(post.c_str());
			}
			else{
				cout << "Command \'" << pre << "\' not recognized. Please reference the manual." << endl; 
			}
		}
	}

	//set the random seed
	srand(rseed);
	
	//make the Simplex noise object
	SimplexNoise noise(octaves,gain,lacunarity);

	//make the Shader object
	Shader shader(kernel,0.5f,shade,highlight);
	
	//make the height_map
	Array_2D<float> height_map(xgrid,ygrid);
	
	//make the color map
	color_map colmap(xgrid,ygrid);
	
	//get started...
	beginning = time(NULL);
	cout << "Starting up..." << endl;
	
	string fname = fchunk + "000.bmp";
	int len = fchunk.length();

	
	/**********NOISE TRACKING************/
	//loop!
	for (i = 0; i < zgrid; ++i)
	{
		//first, make the filename
		//	there are many (probably easier) ways to do this, but this is efficient and simple
		++fname[len+2];//characters have the useful ability to exceed 9, meaning it's very easy to roll over
		if (fname[len+2] > '9'){++fname[len+1];fname[len+2]='0';}
		if (fname[len+1] > '9'){++fname[len];  fname[len+1]='0';}//this won't work if you exceed 999!
		
		//now make noise!
		noise.makeSomeNoise(height_map,i);
		
		//give it some color
		colorize(height_map,colmap,grad);
		
		//shade it
		shader.render_directional_light(height_map,colmap,phi);
		
		//print out the height_map
		colmap.print(fname);
		
		//finally, update the user (occasionally)
		if ((i & 15) == 15){
			cout << i << " frames done. Estimated time remaining: "
				 << secondsToString(int(double(time(NULL)-beginning) * double(zgrid - i) / (double)i))
				 << ". " << endl;
		}
	}
	//*/

	/**********SHADOW TRACKING***********
	//make a static map first, then each frame renders it differently
	noise.makeSomeNoise(height_map,11);
	if (eroder != NULL)
		eroder->erode(iterations, height_map);

	//loop!
	for (i = 0; i < zgrid; ++i)
	{
		//first, make the filename
		//	there are many (probably easier) ways to do this, but this is efficient and simple
		++fname[len+2];//characters have the useful ability to exceed 9, meaning it's very easy to roll over
		if (fname[len+2] > '9'){++fname[len+1];fname[len+2]='0';}
		if (fname[len+1] > '9'){++fname[len];  fname[len+1]='0';}//this won't work if you exceed 999!
		
		//give it some color
		colorize(height_map,colmap,grad);
		
		//shade it
		shader.render_directional_light(height_map,colmap,phi * (float)i/ (float)zgrid);
		
		//print out the height_map
		colmap.print(fname);
		
		//finally, update the user (occasionally)
		if ((i & 15) == 15){
			cout << i << " frames done. Estimated time remaining: "
				 << secondsToString(int(double(time(NULL)-beginning) * double(zgrid - i) / (double)i))
				 << ". " << endl;
		}
	}
	//*/
	
	//finally (for the user), spit out the total time
	cout << "\nDone. Total time taken: " << secondsToString(int(double(time(NULL) - beginning))) << " for " << i << " frames.\n"
		 << "Average time per frame: " << (double(time(NULL))-double(beginning))/double(i) << " seconds.\n" 
		 << "The seed was "<< rseed << '.' << endl;

	return (0);
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
