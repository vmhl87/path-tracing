#pragma once

#include <vector>

struct sphere{
	transform t;
	material m;
	// bbox b;
	vec final_pos;

	double r;

	void init(){
		final_pos = t.apply({0, 0, 0});
		// vec p = t.apply(vec{0, 0, 0});
		// b.min = p - vec{r, r, r};
		// b.max = p + vec{r, r, r};
	}

	bool hit(ray &o, touch &res) const{
		// if(b.outside(o)) return false;
		//bool x = hit(t.revert(o.p), t.revert_d(o.d), res);
		//if(x) res.n = t.apply_d(res.n), res.p = t.apply(res.p);
		//return x;
		bool x = hit(o.p-final_pos, o.d, res);
		if(x) res.n += final_pos;
		return x;
	}

	bool hit(vec lp, vec ld, touch &res) const{
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
	bbox b;

	double w, l;

	void init(){
		t.init();

		b({
			t.apply({-w, 0, -l}),
			t.apply({w, 0, -l}),
			t.apply({-w, 0, l}),
			t.apply({w, 0, l}),
		});
	}

	bool hit(ray &o, touch &res) const{
		if(b.outside(o)) return false;
		bool x = hit(t.revert(o.p), t.revert_d(o.d), res);
		if(x) res.n = t.apply_d(res.n), res.p = t.apply(res.p);
		return x;
	}

	bool hit(vec lp, vec ld, touch &res) const{
		if(std::abs(ld.y) <= 0.001) return false;
		if(copysign(1.0, ld.y) == copysign(1.0, lp.y)) return false;
		res.d = -ld.mag()/ld.y * lp.y;

		res.p = {lp.x+ld.x*res.d, copysign(1e-6, lp.y), lp.z+ld.z*res.d};
		if(std::abs(res.p.x) > w || std::abs(res.p.z) > l) return false;

		res.n = {0, copysign(1.0, lp.y), 0};
		res.m = &m;

		return true;
	}
};

std::vector<rect> rects;

struct _obj_template{
	transform t;
	material m;
	bbox b;

	bool hit(ray &o, touch &res) const{
		if(b.outside(o)) return false;
		bool x = hit(t.revert(o.p), t.revert_d(o.d), res);
		if(x) res.n = t.apply_d(res.n), res.p = t.apply(res.p);
		return x;
	}

	bool hit(vec lp, vec ld, touch &res) const;
};

bool hit(ray &r, touch &t, bool use_light = false){
	touch alt; t.d = 1e18;
	bool res = 0;

	for(const sphere &s : spheres) if(use_light || !s.m.light)
		if(s.hit(r, alt) && alt.d < t.d)
			t = alt, res |= 1;

	for(const rect &s : rects) if(use_light || !s.m.light)
		if(s.hit(r, alt) && alt.d < t.d)
			t = alt, res |= 1;

	return res;
}
