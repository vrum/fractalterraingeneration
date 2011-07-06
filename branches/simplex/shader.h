#ifndef SHADER_H
#define SHADER_H
#include "color.h"
#include "Array2D.h"

class Shader{
	int kernel_radius;
	float cutoff;
	byte shade;
	byte highlight;
	
	void vertical_blur(color_map& shademap);
	void horizontal_blur(color_map& shademap);
	
public:
	Shader(int kernel, float flood_level, byte Shade, byte Highlight);
	void render_light(Array_2D<float>& height_map, color_map& colmap);
	void render_shadow(Array_2D<float>& height_map, color_map& colmap);
	void render_highlight(Array_2D<float>& height_map, color_map& colmap);

	void render_directional_light(Array_2D<float>& height_map, color_map& colmap, float phi);
};

#endif
