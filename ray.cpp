#pragma once

#include <vector>

struct ray{
	vec p, d;
	color c;
};

struct material{
	bool light = 0;
	color c;

	double specular = 0.0;
	double gloss = 0.0;
};

struct touch{
	const material *m;
	vec n, p;
	double d;

	void scatter(ray &in){
		in.p = p + n*1e-6;

		if(rng::base() < m->specular){
			vec out = in.d - n*2.0*in.d.dot(n);
			if(m->gloss < 1e-6) in.d = out;
			else{
				in.d = (out + rng::uniform_norm()*m->gloss).norm();
				if(in.d.dot(n) < 0.0)
					in.d = in.d - n*2.0*in.d.dot(n);
			}

		}else{
			// lambert (cosine) distribution
			in.d = (rng::uniform_norm() + n).norm();
		}
	}
};

struct sphere{
	transform t;
	material m;
	double r;

	bool hit(ray &o, touch &res) const{
		bool x = _hit(t.revert(o.p), t.revert_d(o.d), res);

		if(x) res.n = t.apply_d(res.n),
			res.p = t.apply(res.p);

		return x;
	}

	bool _hit(vec lp, vec ld, touch &res) const{
		double b = 2.0*lp.dot(ld),
			   c = lp.dot(lp)-r*r,
			   D = b*b - 4*c;

		if(D<0) return false;
		else{
			double t1 = (-b-std::sqrt(D)) / 2.0,
				   t2 = (-b+std::sqrt(D)) / 2.0;

			if(t1 > 0.001) res.d = t1;
			else if(t2 > 0.001) res.d = t2;
			else return false;

			res.n = (res.p = lp+ld*res.d).norm(),
			res.m = &m;

			return true;
		}
	}
};

std::vector<sphere> spheres;

struct rect{
	transform t;
	material m;
	double w, l;
	bool bias = false;

	bool hit(ray &o, touch &res) const{
		bool x = _hit(t.revert(o.p), t.revert_d(o.d), res);

		if(x) res.n = t.apply_d(res.n),
			res.p = t.apply(res.p);

		return x;
	}

	bool _hit(vec lp, vec ld, touch &res) const{
		if(ld.y == 0.0) return false;
		if(lp.y < 0.0 && bias) return false;
		if(copysign(1.0, ld.y) == copysign(1.0, lp.y)) return false;
		res.d = -ld.mag()/ld.y * lp.y;

		vec p = lp + ld*res.d;
		if(std::abs(p.x) > w || std::abs(p.z) > l) return false;

		double eps = 1e-8;
		res.n = {0, copysign(1.0, lp.y), 0};
		res.p = p + res.n*eps;
		res.m = &m;

		return true;
	}
};

std::vector<rect> rects;

struct _obj{
	transform t;
	material m;

	bool hit(ray &o, touch &res) const{
		bool x = _hit(t.revert(o.p), t.revert_d(o.d), res);

		if(x) res.n = t.apply_d(res.n),
			res.p = t.apply(res.p);

		return x;
	}

	bool _hit(vec lp, vec ld, touch &res) const;
};

bool hit(ray &r, touch &t){
	touch alt; t.d = 1e18;
	bool res = 0;

	for(const sphere &s : spheres)
		if(s.hit(r, alt) && alt.d < t.d)
			t = alt, res |= 1;

	for(const rect &s : rects)
		if(s.hit(r, alt) && alt.d < t.d)
			t = alt, res |= 1;

	return res;
}
