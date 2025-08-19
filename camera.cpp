#pragma once

#include "bmp.cpp"

struct _camera{
	vec p;
	transform t;

	int w, h;
	double c;

	int spp = 1, report = 0;
	double exposure = 1.0,
		   gamma = 2.2;

	int bounces = 5;

	color *data = nullptr;
	unsigned char *image = nullptr;

	void init(){
		t.init();
		data = new color[w*h];
		image = new unsigned char[w*h*3];
	}

	void get(double X, double Y, ray &r){
		vec dir = {
			(X-w/2)/c,
			(h/2-Y)/c,
			1.0,
		};

		r.d = t.apply(dir).norm();
		r.c = {1, 1, 1};
		r.p = p;
	}

	void set(int x, int y, color &c){
		data[x+y*w] += c;
	}

	void set(vec &ray, color col){
		vec coord = t.revert(ray-p);
		if(coord.z <= 0.0) return;
		coord /= coord.z;
		coord *= c;
		int x = w/2 + std::floor(coord.x),
			y = h/2 - std::floor(coord.y) - 1;

		if(x >= 0 && x < w && y >= 0 && y < h)
			data[x+y*w] += col;
	}

	void write(double progress = 1.0){
		double exp2 = spp * progress / exposure;

		auto bake = [&] (double v){
			return 255.0 * pow(v/exp2, 1.0/gamma);
		};

		auto c = [&] (double v) {
			return (unsigned char) std::max(0, std::min(255,
						(int) std::floor(bake(v))));
		};

		for(int i=0; i<w*h; ++i){
			image[i*3+2] = c(data[i].x);
			image[i*3+1] = c(data[i].y);
			image[i*3] = c(data[i].z);
		}

		writeBMP("image.bmp", image, w, h);
	}
};

_camera camera;
