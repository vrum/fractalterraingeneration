#ifndef COLORIZER_H
#define COLORIZER_H

#include "color.h"
#include "Array2D.h"
#include <map>
#include <string>

typedef std::map<float,color> gradient;

void colorize(const Array_2D<float>& height_map, color_map& ret_val, gradient& colors);

//gradient helper functions
void make_gradient(gradient& grad, std::string post);
void print_gradient(gradient& grad);

#endif
