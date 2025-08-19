#pragma once

struct ray{
	vec p, d;
	color c;
};

struct material{
	bool light = false;
	color c;
	double smooth = 1.0;
};

vec cosine_lobe(double m, vec n){
	double costheta = std::pow(rng::base(), 1.0 / (1.0 + m)),
		   sintheta = std::sqrt(1.0 - costheta*costheta);

	double sign = copysign(1.0, n.z);
	double a = -1.0 / (sign + n.z);
	double b = n.x * n.y * a;

	double phi = rng::base() * M_PI * 2.0,
		   cosphi = cos(phi), sinphi = sin(phi);

	return (vec) {
		sintheta * ((1.0 + sign*n.x*n.x*a)*cosphi + b*sinphi) + n.x*costheta,
		sintheta * (sign*b*cosphi + (sign+n.y*n.y*a)*sinphi) + n.y*costheta,
		n.z*costheta - sintheta * (sign*n.x*cosphi + n.y*sinphi),
	};
}

double cosine_dist(double m, vec n, vec v){
	return std::pow(std::max(0.0, v.dot(n)), m) * (m + 1.0);
}

struct touch{
	const material *m;
	vec n, p;
	double d;

	void scatter(ray &in){
		in.p = p;

		vec surface = cosine_lobe(m->smooth, n);
		in.d = in.d - surface*2.0*in.d.dot(surface);
		if(in.d.dot(n) < 0.0) in.d = in.d - n*2.0*in.d.dot(n);
	}

	double scatter(vec in, vec out){
		vec s1 = (out-in).norm(), s2 = (out-n*2.0*out.dot(n)-in).norm();
		return cosine_dist(m->smooth, n, s1) + cosine_dist(m->smooth, n, s2);
	}
};
