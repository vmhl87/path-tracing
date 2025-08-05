#pragma once

#include <cmath>

#include "bmp.cpp"

struct vec{
	double x, y, z;

	vec operator+(const vec o){
		return {x+o.x, y+o.y, z+o.z};
	}

	vec operator-(const vec o){
		return {x-o.x, y-o.y, z-o.z};
	}

	vec operator*(double v){
		return {x*v, y*v, z*v};
	}

	vec operator/(double v){
		return {x/v, y/v, z/v};
	}

	double mag(){
		return std::sqrt(x*x + y*y + z*z);
	}

	vec norm(){
		double m = mag();
		return {x/m, y/m, z/m};
	}

	double dot(const vec o){
		return x*o.x + y*o.y + z*o.z;
	}

	vec cross(const vec o){
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
};

using color = vec;

struct material{
	double shine;
	color c;
};

struct ray{
	vec p, d;
	color c;
};

struct img{
	color *dat;
	int w, h;

	void init(int W, int H){
		w = W, h = H;
		dat = new color[w*h];
	}

	~img() { delete[] dat; }

	color& at(int x, int y){
		return dat[x+y*w];
	}

	void write(const char *fname){
		unsigned char *d = new unsigned char[w*h*3];

		// different energy function
		auto bake = [] (double v){
			return v * 255.0;
		};

		auto c = [&bake] (double v) {
			return (unsigned char) std::max(0, std::min(255,
						(int) std::floor(bake(v))));
		};

		for(int i=0; i<w*h; ++i){
			d[i*3+2] = c(dat[i].x);
			d[i*3+1] = c(dat[i].y);
			d[i*3] = c(dat[i].z);
		}

		writeBMP(fname, d, w, h);
		delete[] d;
	}
};

struct dir{
	vec f, u;
};

struct camera{
	vec p;
	dir d;
	int w, h;
	double c;
	img image;

	void init(){
		image.init(w, h);
	}

	// save_raw_image: in order to post-process brightness, etc

	void write(const char *fname){
		image.write(fname);
	}
};
