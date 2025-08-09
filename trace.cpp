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

double tries, finish;

void forward_trace(ray &r, int iter){
	touch t = hit(r);

	if(t.hit){
		r.c *= t.mat.c;

		ray R;
		R.p = t.p; R.t = r.t;
		R.d = (camera.p-t.p).norm();

		vec sf = (R.d-r.d).norm();
		R.c = r.c * std::pow(sf.dot(t.norm), t.mat.shine) * (t.mat.shine + 1.0);
		add(R);

		r.p = t.p;

		while(r.d.dot(t.norm) < 0.0){
			double costheta = std::pow(rng::base(), 1.0 / (1.0 + t.mat.shine)),
				   sintheta = std::sqrt(1.0 - costheta*costheta);

			vec par = rng::uniform_norm();
			par = (par - t.norm*par.dot(t.norm)).norm();

			vec surface = par*sintheta + t.norm*costheta;

			r.d = r.d - surface * 2.0 * r.d.dot(surface);

			++tries;
		}

		++finish;

		if(iter > 0) forward_trace(r, iter-1);
	}
}

bool backward_trace(ray &r, int iter){
	touch t = hit(r);

	if(t.hit){
		r.c *= t.mat.c;

		r.p = t.p;

		while(r.d.dot(t.norm) < 0.0){
			double costheta = std::pow(rng::base(), 1.0 / (1.0 + t.mat.shine)),
				   sintheta = std::sqrt(1.0 - costheta*costheta);

			vec par = rng::uniform_norm();
			par = (par - t.norm*par.dot(t.norm)).norm();

			vec surface = par*sintheta + t.norm*costheta;

			r.d = r.d - surface * 2.0 * r.d.dot(surface);

			++tries;
		}

		++finish;

		if(iter > 0) return backward_trace(r, iter-1);

		return false;

	}else{
		r.c *= r.d.dot(global_dir)*global_mag + 1.0-global_mag;
		return true;
	}
}
