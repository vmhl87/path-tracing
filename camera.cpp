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

	int threads = 1;
	color *data[MAX_THREADS] = {};
	unsigned char *image = nullptr;

	void init(){
		t.init();
		image = new unsigned char[w*h*3];
	}

	void init(int id){
		data[id] = new color[w*h];
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

	void set(int id, int x, int y, color &c){
		data[id][x+y*w] += c;
	}

	void set(int id, vec &ray, color col){
		vec coord = t.revert(ray-p);
		if(coord.z <= 0.0) return;
		coord /= coord.z;
		coord *= c;
		int x = w/2 + std::floor(coord.x),
			y = h/2 - std::floor(coord.y) - 1;

		if(x >= 0 && x < w && y >= 0 && y < h)
			data[id][x+y*w] += col;
	}

	void write(double progress = 1.0){
		double exp2 = threads * spp * progress / exposure;

		for(int i=1; i<threads; ++i)
			for(int j=0; j<w*h; ++j)
				data[0][j] += data[i][j];

		auto bake = [&] (double v){
			return 255.0 * pow(v/exp2, 1.0/gamma);
		};

		auto c = [&] (double v) {
			return (unsigned char) std::max(0, std::min(255,
						(int) std::floor(bake(v))));
		};

		for(int i=0; i<w*h; ++i){
			image[i*3+2] = c(data[0][i].x);
			image[i*3+1] = c(data[0][i].y);
			image[i*3] = c(data[0][i].z);
		}

		writeBMP("image.bmp", image, w, h);
	}
};

_camera camera;
