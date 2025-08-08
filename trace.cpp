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
		//R.c = r.c * R.d.dot(t.norm);
		//R.c = r.c;
		//add(R);

		/*
		vec sf = (R.d-r.d).norm();
		sf = sf.lerp(t.norm, -t.mat.shine/(1.0-t.mat.shine)).norm();
		vec x = r.d - sf * 2.0 * r.d.dot(sf);
		double y = x.dot(t.norm);
		R.c = r.c * y * 3.14;
		//R.c = r.c * 3.14;
		if(y > 0.0) add(R);
		*/

		double vol = M_PI * 0.5 / (9.0 * t.mat.shine + 3.0);

		vec sf = (R.d-r.d).norm();

		// better expansion
		//sf = sf.lerp(t.norm, -1);
		/*
		double Y = sf.dot(t.norm),
			   X = std::sqrt(1.0 - Y*Y),
			   Z = (Y*Y-0.5)/Y;
		sf = (sf + t.norm*(Z-Y)).norm();
		*/

		//R.c = r.c * std::pow(sf.dot(t.norm), t.mat.shine) / vol;
		double D = sf.dot(t.norm); D = std::max(0.0, 2.0*D*D - 1.0);
		R.c = r.c * std::pow(D, t.mat.shine) / vol;

		add(R);
		//vec x = r.d - sf * 2.0 * r.d.dot(sf);
		//if(y > 0.0) add(R);

		r.p = t.p;

		// lambertian
		//r.d = (t.norm+rng::uniform_norm()).norm();

		//r.d = rng::uniform_norm();
		//if(r.d.dot(t.norm) < 0.0) r.d *= -1;

		//vec surface = ((t.norm+rng::uniform_norm()).norm()-r.d).norm();
		//surface = surface.lerp(t.norm, t.mat.shine).norm();
		//r.d = r.d - surface * 2.0 * r.d.dot(surface);

		vec surface = rng::uniform_norm();
		if(surface.dot(t.norm) < 0.0) surface *= -1;

		// importance sampling..
		//r.c *= std::pow(surface.dot(t.norm), t.mat.shine) / vol;
		double d = surface.dot(t.norm); d = std::max(0.0, 2.0*d*d - 1.0);
		r.c *= std::pow(d, t.mat.shine) / vol;

		// better contraction
		//surface = surface.lerp(t.norm, 0.5).norm();
		/*
		double y = surface.dot(t.norm),
			   z = std::sqrt((1.0+y) / (1.0-y));
		surface = (surface + t.norm*(z-y)).norm();
		*/

		//surface = (surface-r.d).norm();
		r.d = r.d - surface * 2.0 * r.d.dot(surface);

		if(iter > 0) forward_trace(r, iter-1);
	}
}

bool backward_trace(ray &r, int iter){
	touch t = hit(r);

	if(t.hit){
		r.c *= t.mat.c;

		r.p = t.p;

		//vec surface = ((t.norm+rng::uniform_norm()).norm()-r.d).norm();
		
		double vol = M_PI * 0.5 / (9.0 * t.mat.shine + 3.0);
		
		vec surface = rng::uniform_norm();
		if(surface.dot(t.norm) < 0.0) surface *= -1;
		
		// importance sampling..
		//r.c *= std::pow(surface.dot(t.norm), t.mat.shine) / vol;
		double d = surface.dot(t.norm); d = std::max(0.0, 2.0*d*d - 1.0);
		r.c *= std::pow(d, t.mat.shine) / vol;

		// better contraction
		//surface = surface.lerp(t.norm, 0.5).norm();
		/*
		double y = surface.dot(t.norm),
			   z = std::sqrt((1.0+y) / (1.0-y));
		surface = (surface + t.norm*(z-y)).norm();
		*/

		//surface = (surface-r.d).norm();
		r.d = r.d - surface * 2.0 * r.d.dot(surface);

		//surface = surface.lerp(t.norm, t.mat.shine).norm();
		//r.d = r.d - surface * 2.0 * r.d.dot(surface);

		if(iter > 0) return backward_trace(r, iter-1);

		return false;

	}else{
		r.c *= r.d.dot(global_dir)*global_mag + 1.0-global_mag;
		return true;
	}
}
