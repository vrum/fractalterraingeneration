#include "shader.h"
#include "color.h"
#include "Array2D.h"
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

Shader::Shader(int kernel,float flood_level, byte Shade, byte Highlight){
	if (kernel <= 0)
		kernel_radius = 1;
	else if ((kernel & 1) == 0)
		kernel_radius = (kernel)/2;
	else
		kernel_radius=(kernel-1)/2;

	cutoff = flood_level;
	shade = Shade;
	highlight = Highlight;
}

void Shader::render_highlight(Array_2D<float>& height_map, color_map& colmap){
	render_shadow(height_map,colmap);
	render_highlight(height_map,colmap);
}

void Shader::render_shadow(Array_2D<float>& height_map, color_map& colmap){

}

void Shader::render_light(Array_2D<float>& height_map, color_map& colmap){
	unsigned i,j;
	
	//make the shade map
	color_map shademap(colmap.width(), colmap.height());
	
	//find out where the flood level is
	float max,min,flood_level;
	max=min=height_map(0,0);
	for (i=0;i<colmap.width();++i){
		for (j=0;j<colmap.height();++j){
			if (height_map(i,j) < min) min=height_map(i,j);
			if (height_map(i,j) > max) max=height_map(i,j);
		}
	}
	flood_level = (max+min) * cutoff;
	
	//highlight
	color base(0,0,0,0);
	for (j=0;j<colmap.height(); ++j)
		shademap(0,j) = base;
		
	for (i = 1; i < colmap.width(); ++i){
		for (j = 0; j < colmap.height(); ++j){
			//if this point is above the cutoff and it is lower than the left-neighboring point
			if (height_map(i,j) > flood_level && height_map(i,j) < height_map(i-1,j))
				shademap(i,j) = color(highlight,highlight,highlight,0);
			else
				shademap(i,j) = base;
		}
	}
	
	//blur it
	vertical_blur(shademap);
	horizontal_blur(shademap);
	
	//add it to the previous map
	colmap.value_addition(shademap);
	
	//shadows
	unsigned w = colmap.width() - 1;
	base = color(0,0,0,0);
	for (j=0;j<colmap.height(); ++j)
		shademap(w,j) = base;
		
	for (i = 0; i < w; ++i){
		for (j = 0; j < colmap.height(); ++j){
			//if this point is above the cutoff and it is lower than the right neighboring point...
			if (height_map(i,j) > flood_level && height_map(i,j) < height_map(i+1,j))
				shademap(i,j) = color(shade,shade,shade,0);
			else
				shademap(i,j) = base;
		}
	}
	
	//blur it
	vertical_blur(shademap);
	horizontal_blur(shademap);
	//vertical_blur(shademap);
	//horizontal_blur(shademap);
	
	//subtract it from the previous map
	colmap.value_subtraction(shademap);
}

void Shader::vertical_blur(color_map& shademap){
	//copy the shademap
	int i,j,k,y,t, window_total[4], kernel_area,
		width = shademap.width(),
		height = shademap.height();
	color_map sourcemap(shademap);
	
	for (i = 0; i < width; ++i){
		for (j = 0; j < height; ++j){
			//add up the values from the kernel area
			window_total[0]=window_total[1]=window_total[2]=window_total[3]=0;
			kernel_area = 0;
			for (y=-kernel_radius; y <= kernel_radius; ++y){
				t=j+y;
				if (t>=0 && t < height){
					for (k=0;k<4;++k)
						window_total[k] += sourcemap(i,t).v[k];
					++kernel_area;
				}
			}
			
			//get the average
			shademap(i,j).v[0] = (byte)(window_total[0] / kernel_area);
			shademap(i,j).v[1] = (byte)(window_total[1] / kernel_area);
			shademap(i,j).v[2] = (byte)(window_total[2] / kernel_area);
			shademap(i,j).v[3] = (byte)(window_total[3] / kernel_area);
		}
	}
}

void Shader::horizontal_blur(color_map& shademap){
	//copy the shademap
	int i,j,k,x,t, window_total[4], kernel_area,
		width = shademap.width(),
		height = shademap.height();
	color_map sourcemap(shademap);

	for (j = 0; j < height; ++j){
		for (i = 0; i < width; ++i){
			//add up the values from the kernel area
			window_total[0]=window_total[1]=window_total[2]=window_total[3]=0;
			kernel_area = 0;
			
			for (x=-kernel_radius; x <= kernel_radius; ++x){
				t=i+x;
				
				if (t>=0 && t < width){
					for (k=0;k<4;++k)
						window_total[k] += sourcemap(t,j).v[k];
					++kernel_area;
				}
			}
			
			//get the average
			shademap(i,j).v[0] = (byte)(window_total[0] / kernel_area);
			shademap(i,j).v[1] = (byte)(window_total[1] / kernel_area);
			shademap(i,j).v[2] = (byte)(window_total[2] / kernel_area);
			shademap(i,j).v[3] = (byte)(window_total[3] / kernel_area);
		}
	}
}

void Shader::render_directional_light(Array_2D<float>& height_map, color_map& colmap, float phi){
	//set up some variables...
	//float phi = .26f; //roughly 15 degrees
	const unsigned xgrid = colmap.width(),
				   ygrid = colmap.height();
	unsigned x,y,xoff;
	float z_offset = tanf(phi);
	float current_height;
	color_map shade_map(xgrid,ygrid);

	//find out where the flood level is
	float max,min,flood_level;
	max=min=height_map(0,0);
	for (x=0;x<xgrid;++x){
		for (y=0;y<ygrid;++y){
			if (height_map(x,y) < min) min = height_map(x,y);
			if (height_map(x,y) > max) max = height_map(x,y);
		}
	}
	flood_level = (max+min) * cutoff;

	//for each pixel...
	for (x = 0; x < xgrid; ++x){
		for (y = 0; y < ygrid; ++y){
			current_height = height_map(x,y);

			//make water act like a flat plane
			if (current_height < flood_level)
				current_height = flood_level;

			//go right until either the edge is met or a point
			for (xoff = 1; (xoff+x) < xgrid; ++xoff){
				//moved right, now move up
				current_height += z_offset;

				//if we meet a point that is tall enough...
				if (height_map(x + xoff,y) > current_height){ //realistically this should be >=, but it's kind of a moot point with floats...
					shade_map(x, y) = color(shade,shade,shade,255);

					break;
				}
			}
		}
	}
	vertical_blur(shade_map);
	horizontal_blur(shade_map);
	
	//subtract the shade map from the color map
	colmap.value_subtraction(shade_map);
}
