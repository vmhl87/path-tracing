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

touch hit(ray &r){
	touch best; best.d = 1e18, best.hit = 0;

	for(const sphere &s : spheres){
		touch h = s.hit(r);
		if(h.hit && h.d < best.d) best = h;
	}

	return best;
}

void add(ray &r){
	touch t = hit(r);
	if(t.hit && t.d <= r.p.dist(camera.p)) return;
	camera.add(r.p, r.c);
}

void trace(ray &r, int iter){
	touch t = hit(r);

	if(t.hit){
		r.c *= t.mat.c;

		ray R;
		
		R.p = t.p;
		R.d = (camera.p-t.p).norm();
		R.c = r.c * R.d.dot(t.norm);

		add(R);

		r.p = t.p;
		r.d = r.d - t.norm * 2.0 * r.d.dot(t.norm);

		// shine should change this distribution
		r.d = (r.d+random_vec()).norm();

		if(iter > 0) trace(r, iter-1);
	}
}
