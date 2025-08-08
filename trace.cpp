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
	vec p, d;

	material mat;

	void hit(const ray &o, touch &res) const{
		vec P = p + d*o.t;

		vec oc = o.p-P;

		double b = 2.0*oc.dot(o.d),
			   c = oc.dot(oc)-r*r,
			   D = b*b - 4*c;

		if(D<0) res.hit = false;
		else{
			double t1 = (-b-std::sqrt(D)) / 2.0,
				   t2 = (-b+std::sqrt(D)) / 2.0;

			if(t1 > 0.001) res.d = t1, res.hit = true;
			else if(t2 > 0.001) res.d = t2, res.hit = true;
			else res.hit = false;

			if(res.hit)
				res.norm = ((res.p = o.p+o.d*res.d)-P).norm(),
				res.mat = mat;
		}
	}
};

std::vector<sphere> spheres;

cam camera;

touch hit(ray &r){
	touch best, alt; best.d = 1e18, best.hit = 0;

	for(const sphere &s : spheres){
		s.hit(r, alt);
		if(alt.hit && alt.d < best.d) best = alt;
	}

	return best;
}

void add(ray &r){
	touch t = hit(r);
	if(t.hit && t.d <= r.p.dist(camera.p)) return;
	camera.add(r.p, r.c);
}

void forward_trace(ray &r, int iter){
	touch t = hit(r);

	if(t.hit){
		r.c *= t.mat.c;

		ray R;
		R.p = t.p; R.t = r.t;
		R.d = (camera.p-t.p).norm();

		// lambertian
		R.c = r.c * R.d.dot(t.norm);
		//R.c = r.c;
		add(R);

		/*
		vec sf = (R.d-r.d).norm();
		sf = sf.lerp(t.norm, -t.mat.shine/(1.1-t.mat.shine)).norm();
		vec x = r.d - sf * 2.0 * r.d.dot(sf);
		double y = x.dot(t.norm);
		R.c = r.c * y;
		if(t.mat.shine > .5) R.c *= 10;
		if(y > 0.0) add(R);
		*/

		r.p = t.p;

		// lambertian
		r.d = (t.norm+rng::uniform_norm()).norm();

		//r.d = rng::uniform_norm();
		//if(r.d.dot(t.norm) < 0.0) r.d *= -1;

		/*
		vec surface = ((t.norm+rng::uniform_norm()).norm()-r.d).norm();
		surface = surface.lerp(t.norm, t.mat.shine).norm();
		r.d = r.d - surface * 2.0 * r.d.dot(surface);
		*/

		if(iter > 0) forward_trace(r, iter-1);
	}
}

bool backward_trace(ray &r, int iter){
	touch t = hit(r);

	if(t.hit){
		r.c *= t.mat.c;

		r.p = t.p;

		vec surface = ((t.norm+rng::uniform_norm()).norm()-r.d).norm();
		
		//vec surface = rng::uniform_norm();
		//if(surface.dot(t.norm) < 0.0) surface *= -1;

		surface = surface.lerp(t.norm, t.mat.shine).norm();
		r.d = r.d - surface * 2.0 * r.d.dot(surface);

		if(iter > 0) return backward_trace(r, iter-1);

		return false;

	}else{
		r.c *= r.d.dot(global_dir)*global_mag + 1.0-global_mag;
		return true;
	}
}
