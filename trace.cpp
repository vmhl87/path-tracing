#pragma once

#include <vector>

#include "util.cpp"

struct sphere{
	double r;
	vec p;

	material mat;
};

std::vector<sphere> spheres;

cam camera;
