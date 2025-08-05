#pragma once

#include <vector>

#include "util.cpp"

struct sphere{
	double r;
	vec p;

	material mat;
};

std::vector<sphere> spheres;

bool intersect(const ray &r, const sphere &s, double *t_out = nullptr) {
    vec oc = r.p - s.p;

    double a = r.d.dot(r.d);

    double b = 2.0 * r.d.dot(oc);

    double c = oc.dot(oc) - (s.r * s.r);

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;
    }

    double t0 = (-b - std::sqrt(discriminant)) / (2.0 * a);
    double t1 = (-b + std::sqrt(discriminant)) / (2.0 * a);

    if (t0 >= 0) {
        if (t_out) {
            *t_out = t0;
        }
        return true;
    }

    if (t1 >= 0) {
        if (t_out) {
            *t_out = t1;
        }
        return true;
    }

    return false;
}

cam camera;

void trace(ray &r, int iter){
	for(const sphere &s : spheres){
		double d;

		if(intersect(r, s, &d)){
			r.c *= s.mat.c;
		}
	}

	if(iter) trace(r, iter-1);
}
