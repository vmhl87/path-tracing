#pragma once

#include <vector>

#include "util.cpp"

struct touch{
	material mat;
	vec norm, p;
	double d;
	bool hit;
};

struct sphere{
	double r;
	vec p;

	material mat;

	touch hit(const ray &o) const{
		vec oc = o.p-p;

		double b = 2.0*oc.dot(o.d),
			   c = oc.dot(oc)-r*r,
			   D = b*b - 4*c;

		touch res;

		if(D<0) res.hit = false;
		else{
			double t1 = (-b-std::sqrt(D)) / 2.0,
				   t2 = (-b+std::sqrt(D)) / 2.0;

			if(t1 > 0.001) res.d = t1, res.hit = true;
			else if(t2 > 0.001) res.d = t2, res.hit = true;
			else res.hit = false;

			if(res.hit)
				res.norm = ((res.p = o.p+o.d*res.d)-p).norm(),
				res.mat = mat;
		}

		return res;
	}
};

std::vector<sphere> spheres;

cam camera;

void trace(ray &r, int iter){
	touch best; best.d = 1e18, best.hit = 0;

	for(const sphere &s : spheres){
		touch h = s.hit(r);
		if(h.hit && h.d < best.d) best = h;
	}

	if(best.hit){
		r.c *= best.mat.c;
		color c = r.c;

		r.p = best.p;
		r.d = r.d - best.norm * 2.0 * r.d.dot(best.norm);

		if(iter > 0) trace(r, iter-1);

		r.c = lerp(c, r.c, best.mat.shine);
	}
}
