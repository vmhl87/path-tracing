#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
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

	std::string out() const{
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

	double base(){
		return rng::uniform_gen(rng::gen);
	}

	double norm(){
		return rng::gaussian_gen(rng::gen);
	}

	vec uniform(){
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

color global = {0, 0, 0};
vec global_dir = {0, 0, 0};
double global_mag = 0.0;
bool use_global = 0;

struct ray{
	double t;
	vec p, d;
	color c;

	std::string out(){
		std::stringstream s;
		s << "{p: " << p.out() << ", d: " << d.out()
			<< ", c: " << c.out() << '}';
		return s.str();
	}
};

struct noisy_vec{
	double r = 0;
	vec p;
	int t = -1; // 0: uniform, 1: uniform_norm, 2: gaussian
	
	vec operator()() const{
		if(t == 0) return p + rng::uniform()*r;
		if(t == 1) return p + rng::uniform_norm()*r;
		if(t == 2) return p + rng::gaussian()*r;
		return p;
	}
};

struct light{
	noisy_vec p, d;
	color c = {1, 1, 1};
	int granular = 1, visible = 0;

	void operator()(ray &r) const{
		r.p = p(), r.d = d().norm();
		r.c = c * (double) granular;
	}
};

std::vector<light> lights;

struct material{
	double shine = 0, gloss = 0;
	color c;

	std::string out(){
		std::stringstream s;
		s << "{c: " << c.out() << ", shine: " << shine << ", gloss: " << gloss << '}';
		return s.str();
	}
};

struct dir{
	vec f, u, s;

	void init(){
		s = u.cross(f).norm();
		u = f.cross(s).norm();
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
	int w, h, *counts;
	double c;
	color *dat = nullptr;
	unsigned char *image = nullptr;
	
	// settings
	int64_t iter = 500,
			report = 0,
			chunk = 10000;
	int bounces = 5;
	double exposure = 1.0,
		   gamma = 2.2;
	int adjust = 1;
	bool raw_output = 0;

	std::string iname = "image.bmp",
				rname = "image.raw";

	void init(){
		d.init();
		dat = new color[w*h];
		if(!raw_output) image = new unsigned char[w*h*3];
		if(use_global) counts = new int[w*h];  // TODO change if bidirectional
	}

	void pick(int &x, int &y){
		int x1 = std::floor(rng::base()*w),
			y1 = std::floor(rng::base()*h),
			x2 = std::floor(rng::base()*w),
			y2 = std::floor(rng::base()*h);

		//if(counts[x1+y1*w] > counts[x2+y2*w]) x = x2, y = y2;
		if((double)counts[x1+y1*w]/(counts[x2+y2*w]+counts[x1+y1*w]) < rng::base())
			x = x2, y = y2;
		else x = x1, y = y1;

		++counts[x+y*w];
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

	void write(bool done, double progress){
		if(raw_output || done){
			std::ofstream raw(rname.c_str());

			raw << w << ' ' << h << ' ' << (int64_t) std::floor(iter*chunk*progress) << '\n';

			for(int i=0; i<w*h; ++i){
				for(size_t j=0; j<sizeof(double); ++j)
					raw << ((unsigned char*) &dat[i].x)[j];
				for(size_t j=0; j<sizeof(double); ++j)
					raw << ((unsigned char*) &dat[i].y)[j];
				for(size_t j=0; j<sizeof(double); ++j)
					raw << ((unsigned char*) &dat[i].z)[j];
			}

			raw.close();

		}

		if(!raw_output){
			double exp2 = iter * chunk * progress / w / h / exposure;

			auto bake = [&] (double v){
				return 255.0 * pow(v/exp2, 1.0/gamma);
			};

			auto c = [&] (double v) {
				return (unsigned char) std::max(0, std::min(255, (int) std::floor(bake(v))));
			};

			for(int i=0; i<w*h; ++i){
				image[i*3+2] = c(dat[i].x);
				image[i*3+1] = c(dat[i].y);
				image[i*3] = c(dat[i].z);
			}

			writeBMP(iname.c_str(), image, w, h);
		}
	}
};
