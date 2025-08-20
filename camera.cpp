#pragma once

#include "bmp.cpp"

struct buffer{
	color *data;

	void init();
	const color operator[](size_t i) const;
	void operator+=(buffer &o);
	void add(int x, int y, color c);
};

struct _camera{
	vec p;
	transform t;

	int w, h;
	double c;

	int spp = 1, report = 0;
	double exposure = 1.0,
		   gamma = 2.2;

	int bounces = 5;

	int threads = 1;

	void get(double X, double Y, ray &r) const{
		vec dir = {
			(X-w/2)/c,
			(h/2-Y)/c,
			1.0,
		};

		r.d = t.apply(dir).norm();
		r.c = {1, 1, 1};
		r.p = p;
	}

	void set(buffer &b, int x, int y, color col) const{
		b.add(x, y, col);
	}

	void set(buffer &b, vec &ray, color col) const{
		vec coord = t.revert(ray-p);
		if(coord.z <= 0.0) return;
		coord /= coord.z;
		coord *= c;
		int x = w/2 + std::floor(coord.x),
			y = h/2 - std::floor(coord.y) - 1;

		if(x >= 0 && x < w && y >= 0 && y < h)
			b.add(x, y, col);
	}
};

_camera camera;

void buffer::init(){
	data = new color[camera.w*camera.h];
}

const color buffer::operator[](size_t i) const{ return data[i]; }

void buffer::operator+=(buffer &o){
	for(int i=0; i<camera.w*camera.h; ++i)
		data[i] += o.data[i];
}

void buffer::add(int x, int y, color c){
	data[x+y*camera.w] += c;
}

struct _target{
	unsigned char *image = nullptr;
	buffer data;

	void init(){
		data.init();
		image = new unsigned char[camera.w*camera.h*3];
	}

	void write(double progress = 1.0){
		double exp2 = camera.threads * camera.spp * progress / camera.exposure;

		auto bake = [&] (double v){
			return 255.0 * pow(v/exp2, 1.0/camera.gamma);
		};

		auto c = [&] (double v) {
			return (unsigned char) std::max(0, std::min(255,
						(int) std::floor(bake(v))));
		};

		for(int i=0; i<camera.w*camera.h; ++i){
			image[i*3+2] = c(data[i].x);
			image[i*3+1] = c(data[i].y);
			image[i*3] = c(data[i].z);
		}

		writeBMP("image.bmp", image, camera.w, camera.h);
	}
};

_target target;
