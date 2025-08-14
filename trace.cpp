#pragma once

#include <vector>

#include "util.cpp"

bool EXPERIMENTAL_RENDER = false;

struct touch{
	material mat;
	vec norm, p;
	double d;
	bool hit;
};

struct sphere{
	double r;
	vec p, d;
	bool blur = false;

	material mat;

	void hit(const ray &o, touch &res) const{
		vec P = blur ? p + d*o.t : p;

		vec oc = o.p-P;

		double b = 2.0*oc.dot(o.d),
			   c = oc.dot(oc)-r*r,
			   D = b*b - 4*c;

		if(D<0) res.hit = false;
		else{
			double t1 = (-b-std::sqrt(D)) / 2.0,
				   t2 = (-b+std::sqrt(D)) / 2.0;

			res.hit = true;

			if(t1 > 0.001) res.d = t1;
			else if(t2 > 0.001) res.d = t2;
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

vec scatter(vec &incoming, vec &normal, double shine){
	double costheta = std::pow(rng::base(), 1.0 / (1.0 + shine)),
		   sintheta = std::sqrt(1.0 - costheta*costheta);

	double sign = copysign(1.0, normal.z);
	double a = -1.0 / (sign + normal.z);
	double b = normal.x * normal.y * a;

	double phi = rng::base() * M_PI * 2.0,
		   cosphi = cos(phi), sinphi = sin(phi);

	vec surface = {
		sintheta * ((1.0 + sign*normal.x*normal.x*a)*cosphi + b*sinphi) + normal.x*costheta,
		sintheta * (sign*b*cosphi + (sign+normal.y*normal.y*a)*sinphi) + normal.y*costheta,
		normal.z*costheta - sintheta * (sign*normal.x*cosphi + normal.y*sinphi),
	};

	return incoming - surface * 2.0 * incoming.dot(surface);
}

double scatter(vec &incoming, vec &outgoing, vec &normal, double shine){
	vec surface = (outgoing-incoming).norm();
	return std::pow(surface.dot(normal), shine) * (shine + 1.0);
}

double scatter2(vec &incoming, vec &outgoing, vec &normal, double shine){
	vec surface = (outgoing-incoming).norm();
	double costheta = surface.dot(normal),
		   theta = std::acos(costheta);
	/*
	double r = shine*4.0*theta/M_PI;
	r = std::max(0.0, 1.0 - r*r);
	double M = M_PI / 4.0 / shine;
	double avg = 1.0 + (1.0-cos(M))*32.0*shine*shine/M_PI/M_PI - sin(M)*8.0*shine/M_PI;
	return r / avg;
	*/
	if(theta > M_PI/4.0/shine) return 0;
	return (0.5 + cos(4.0*shine*theta)/2.0) / 0.089543;
}

void forward_trace(ray &r, double dist = 0.0, int iter = 0){
	touch t = hit(r);

	if(t.hit){
		r.c *= t.mat.c;
		//dist += r.p.dist(t.p);
		if(iter) dist += r.p.dist(t.p);
		r.p = t.p;

		ray R; R.p = t.p; R.t = r.t;
		R.d = (camera.p-t.p).norm();
		R.c = r.c * scatter(r.d, R.d, t.norm, t.mat.shine);

		double dist2 = dist+camera.p.dist(t.p);

		R.c *= 1.0 / dist2 / dist2 * -t.norm.dot(r.d) * t.norm.dot(R.d);

		if(EXPERIMENTAL_RENDER){
			//if(R.c.mag() < 5.0 || !iter) add(R);
			//if(R.c.mag() < 5.0 || !iter) add(R);
			if(t.mat.shine < 50.1 || !iter) add(R);

		}else add(R);
		
		r.c *= -t.norm.dot(r.d);

		while(r.d.dot(t.norm) < 0.0) r.d = scatter(r.d, t.norm, t.mat.shine);

		r.c *= t.norm.dot(r.d);

		if(iter < camera.bounces) forward_trace(r, dist, iter+1);
	}
}

bool backward_trace(ray &r, int iter = 0){
	touch t = hit(r);

	if(t.hit){
		r.c *= t.mat.c;

		r.p = t.p;

		while(r.d.dot(t.norm) < 0.0) r.d = scatter(r.d, t.norm, t.mat.shine);

		if(iter < camera.bounces) return backward_trace(r, iter+1);

		return false;

	}else{
		r.c *= r.d.dot(global_dir)*global_mag + 1.0-global_mag;
		return true;
	}
}
