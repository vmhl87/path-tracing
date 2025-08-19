#pragma once

struct ray{
	vec p, d;
	color c;
};

struct material{
	color c;
	double smooth = 1.0;
};

struct touch{
	const material *m;
	vec n, p;
	double d;

	void scatter(ray &in){
		in.p = p + n*1e-6;

		double costheta = std::pow(rng::base(), 1.0 / (1.0 + m->smooth)),
			   sintheta = std::sqrt(1.0 - costheta*costheta);

		double sign = copysign(1.0, n.z);
		double a = -1.0 / (sign + n.z);
		double b = n.x * n.y * a;

		double phi = rng::base() * M_PI * 2.0,
			   cosphi = cos(phi), sinphi = sin(phi);

		vec surface = {
			sintheta * ((1.0 + sign*n.x*n.x*a)*cosphi + b*sinphi) + n.x*costheta,
			sintheta * (sign*b*cosphi + (sign+n.y*n.y*a)*sinphi) + n.y*costheta,
			n.z*costheta - sintheta * (sign*n.x*cosphi + n.y*sinphi),
		};

		in.d = in.d - surface*2.0*in.d.dot(surface);

		if(in.d.dot(n) < 0.0) in.d = in.d - n*2.0*in.d.dot(n);
	}

	double scatter(vec &in, vec &out){
		vec s1 = (out-in).norm(), s2 = (out-n*2.0*out.dot(n)-in).norm();
		return std::pow(s1.dot(n), m->smooth) * (m->smooth + 1.0) +
			std::pow(s2.dot(n), m->smooth) * (m->smooth + 1.0);
	}
};
