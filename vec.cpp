#pragma once

#include <sstream>
#include <string>
#include <cmath>

#include "headers.hpp"

struct vec{
	double x, y, z;

#define op(oper) inline const vec operator oper(const vec &o) const{ \
	return {x oper o.x, y oper o.y, z oper o.z}; }

	op(+) op(-) op(*) op(/)

#undef op

#define op(oper) inline void operator oper##=(const vec &o){ \
	x oper##= o.x, y oper##= o.y, z oper##= o.z; }

	op(+) op(-) op(*) op(/)

#undef op

#define op(oper) inline const vec operator oper(const double d) const{ \
	return {x oper d, y oper d, z oper d}; }

	op(+) op(-) op(*) op(/)

#undef op

#define op(oper) inline void operator oper##=(const double d){ \
	x oper##= d, y oper##= d, z oper##= d; }

	op(+) op(-) op(*) op(/)

#undef op

	double magsq() const{
		return x*x + y*y + z*z;
	}

	double mag() const{
		return std::sqrt(magsq());
	}

	vec norm() const{
		double m = mag();
		return {x/m, y/m, z/m};
	}

	double dot(const vec o) const{
		return x*o.x + y*o.y + z*o.z;
	}

	vec cross(const vec o) const{
		return {
			y*o.z - z*o.y,
			z*o.x - x*o.z,
			x*o.y - y*o.x,
		};
	}

	vec lerp(const vec o, double v){
		return {
			o.x*v + x*(1.-v),
			o.y*v + y*(1.-v),
			o.z*v + z*(1.-v),
		};
	}

	double distsq(const vec o) const{
		return (*this-o).magsq();
	}

	double dist(const vec o) const{
		return (*this-o).mag();
	}

	std::string out() const{
		auto v = [] (double x) {
			return std::round(x * 1000.0) / 1000.0;
		};

		std::stringstream c;
		c << '{' << v(x) << ' ' << v(y) << ' ' << v(z) << '}';
		return c.str();
	}
};
