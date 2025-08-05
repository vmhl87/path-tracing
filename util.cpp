#pragma once

#include <sstream>
#include <cmath>

#include "bmp.cpp"

struct vec{
	double x, y, z;

#define op(oper) const vec operator oper(const vec &o) const{ \
	return {x oper o.x, y oper o.y, z oper o.z}; \
}

	op(+) op(-) op(*) op(/)

#undef op

#define op(oper) void operator oper##=(const vec &o){ \
	x oper##= o.x, y oper##= o.y, z oper##= o.z; \
}

	op(+) op(-) op(*) op(/)

#undef op

	const vec operator*(const double d) const{
		return {x*d, y*d, z*d};
	}

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

struct img{
	color *dat = nullptr;
	int w, h;

	void init(int W, int H){
		w = W, h = H;
		dat = new color[w*h];
	}

	~img() { if(dat != nullptr) delete[] dat; }

	color& at(int x, int y){
		return dat[x+y*w];
	}
};

struct dir{
	vec f, u;

	void snap(){
		vec s = u.cross(f);
		u = f.cross(s);
	}

	vec project(const vec &v){
		vec s = u.cross(f);
		return {
			f.x*v.z + u.x*v.y + s.x*v.x,
			f.y*v.z + u.y*v.y + s.y*v.x,
			f.z*v.z + u.z*v.y + s.z*v.x,
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
	int w, h, iter = 1;
	double c, noise;
	img image;

	void init(){
		d.snap();
		image.init(w, h);
	}

	// save_raw_image: in order to post-process brightness, etc
	
	void add(int x, int y, color &col){
		image.at(x, y) += col;
	}

	void write(const char *fname){
		unsigned char *d = new unsigned char[w*h*3];

		// different energy function
		auto bake = [&] (double v){
			return v * 255.0 / iter;
		};

		auto c = [&] (double v) {
			return (unsigned char) std::max(0, std::min(255,
						(int) std::floor(bake(v))));
		};

		for(int i=0; i<w*h; ++i){
			d[i*3+2] = c(image.dat[i].x);
			d[i*3+1] = c(image.dat[i].y);
			d[i*3] = c(image.dat[i].z);
		}

		writeBMP(fname, d, w, h);
		delete[] d;
	}
};
