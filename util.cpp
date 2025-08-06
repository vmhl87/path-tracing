#pragma once

#include <iostream>
#include <sstream>
#include <cmath>

#include "bmp.cpp"

struct vec{
	double x, y, z;

#define op(oper) inline const vec operator oper(const vec &o) const{ \
	return {x oper o.x, y oper o.y, z oper o.z}; }

	op(+) op(-) op(*) op(/)

#undef op

#define op(oper) void operator oper##=(const vec &o){ \
	x oper##= o.x, y oper##= o.y, z oper##= o.z; }

	op(+) op(-) op(*) op(/)

#undef op

#define op(oper) inline const vec operator oper(const double d) const{ \
	return {x oper d, y oper d, z oper d}; }

	op(+) op(-) op(*) op(/)

#undef op

#define op(oper) void operator oper##=(const double d){ \
	x oper##= d, y oper##= d, z oper##= d; }

	op(+) op(-) op(*) op(/)

#undef op

	double mag() const{
		return std::sqrt(x*x + y*y + z*z);
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

	double dist(const vec o){
		return (*this-o).mag();
	}

	std::string out(){
		auto v = [] (double x) {
			return std::round(x * 1000.0) / 1000.0;
		};

		std::stringstream c;
		c << '{' << v(x) << ' ' << v(y) << ' ' << v(z) << '}';
		return c.str();
	}
};

vec lerp(vec a, vec b, double c){
	return a.lerp(b, c);
}

namespace rng{
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<double> uniform_gen(0.0, 1.0);
	std::normal_distribution<double> gaussian_gen(0.0, 1.0);

	vec uniform() {
		double u = rng::uniform_gen(rng::gen) * 2.0 - 1.0;
		double phi = rng::uniform_gen(rng::gen) * 2.0 * M_PI;

		double sqrt_1_minus_u2 = sqrt(1.0 - u * u);
		
		vec v = {
			sqrt_1_minus_u2 * cos(phi),
			sqrt_1_minus_u2 * sin(phi),
			u
		};

		double r_cubed = rng::uniform_gen(rng::gen);
		double r = pow(r_cubed, 1.0/3.0);
		
		return v * r;
	}

	vec uniform_norm(){
		double u = rng::uniform_gen(rng::gen) * 2.0 - 1.0;
		double phi = rng::uniform_gen(rng::gen) * 2.0 * M_PI;
		double sqrt_1_minus_u2 = sqrt(1.0 - u * u);

		return {
			sqrt_1_minus_u2 * cos(phi),
			sqrt_1_minus_u2 * sin(phi),
			u
		};
	}

	vec gaussian(){
		vec v = uniform_norm();
		return v * gaussian_gen(rng::gen);
	}
}

using color = vec;

struct material{
	double shine;
	color c;

	std::string out(){
		std::stringstream s;
		s << "{c: " << c.out() << ", shine: " << shine << '}';
		return s.str();
	}
};

struct ray{
	vec p, d;
	color c;

	std::string out(){
		std::stringstream s;
		s << "{p: " << p.out() << ", d: " << d.out()
			<< ", c: " << c.out() << '}';
		return s.str();
	}
};

struct dir{
	vec f, u, s;

	void init(){
		s = u.cross(f);
		u = f.cross(s);
	}

	vec project(const vec &v){
		return {
			f.x*v.z + u.x*v.y + s.x*v.x,
			f.y*v.z + u.y*v.y + s.y*v.x,
			f.z*v.z + u.z*v.y + s.z*v.x,
		};
	}

	vec map(const vec &v){
		return {
			v.dot(s),
			v.dot(u),
			v.dot(f),
		};
	}

	std::string out(){
		std::stringstream s;
		s << "{f: " << f.out() << ", u: " << u.out()
			<< ", s: " << f.cross(u).out() << '}';
		return s.str();
	}
};

struct cam{
	vec p;
	dir d;
	int w, h;
	double c;
	color *dat = nullptr;
	unsigned char *image = nullptr;
	
	// settings
	int64_t iter = 10000,
			report = 0;
	int bounces = 5;
	double exposure = 1.0,
		   gamma = 0.0;

	void init(){
		d.init();
		dat = new color[w*h];
		image = new unsigned char[w*h*3];
	}

	color& at(int x, int y){
		return dat[x+y*w];
	}

	void add(int x, int y, color &col){
		at(x, y) += col;
	}

	void add(vec &ray, color &col){
		vec coord = d.map(ray-p);
		if(coord.z == 0) return;
		coord /= coord.z;
		coord *= c;
		int x = w/2 + std::floor(coord.x),
			y = h/2 - std::floor(coord.y);

		if(x >= 0 && x < w && y >= 0 && y < h)
			at(x, y) += col;
	}

	// save_raw_image: in order to post-process brightness, etc
	
	void write(const char *iname, const char *rname, double progress){
		double exp2 = iter * progress / w / h / exposure;

		auto bake = [&] (double v){
			double energy = v/exp2;
			if(gamma > 1.0) energy = pow(energy, 1.0/gamma);
			return 255.0 * energy;
		};

		auto c = [&] (double v) {
			return (unsigned char) std::max(0, std::min(255,
						(int) std::floor(bake(v))));
		};

		for(int i=0; i<w*h; ++i){
			image[i*3+2] = c(dat[i].x);
			image[i*3+1] = c(dat[i].y);
			image[i*3] = c(dat[i].z);
		}

		writeBMP(iname, image, w, h);
	}
};
