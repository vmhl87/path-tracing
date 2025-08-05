#pragma once

#include "color.cpp"
#include "bmp.cpp"

struct img{
	color *dat;
	int w, h;

	img(int W, int H):w(W),h(H){
		dat = new color[w*h];
	}

	~img() { delete[] dat; }

	color& at(int x, int y){
		return dat[x+y*w];
	}

	void write(const char *fname){
		unsigned char *d = new unsigned char[w*h*3];

		auto c = [] (double v) {
			return (unsigned char) std::max(0, std::min(255,
						(int) std::floor(v*255.0)));
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
