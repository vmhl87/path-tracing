#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>

#include "bmp.cpp"

struct color{
	double r, g, b;
};

int main(){
	std::string s;

	int w = -1, h = -1;
	int64_t samples = 0;

	double exposure = 1.0,
		   gamma = 2.2;

	std::string iname = "";

	color *dat = nullptr;

	while(true){
		std::getline(std::cin, s);
		if(s == "done") break;

		std::istringstream v(s);
		std::string x; v >> x;

		if(x == "exposure") v >> exposure;
		if(x == "gamma") v >> gamma;

		if(x == "write") v >> iname;

		if(x == "read"){
			std::string fname; v >> fname;
			std::ifstream raw(fname);

			std::string header;
			std::getline(raw, header);
			std::istringstream V(header);

			int64_t W, H, Samples;
			V >> W >> H >> Samples;

			if(w != -1 && (W != w || H != h)){
				std::cerr << "mismatched dimensions\n";
				exit(1);
			}

			if(w == -1) dat = new color[W*H];

			w = W, h = H, samples += Samples;

			auto read = [&] () {
				double res;
				for(size_t j=0; j<sizeof(double); ++j){
					char ch; raw.get(ch);
					((unsigned char*) &res)[j] = ch;
				}
				return res;
			};

			for(int i=0; i<w*h; ++i){
				dat[i].r += read();
				dat[i].g += read();
				dat[i].b += read();
			}

			raw.close();
		}
	}

	if(w == -1){
		std::cerr << "no RAW files loaded\n";
		exit(1);
	}

	if(iname == ""){
		std::cerr << "no output file selected\n";
		exit(1);
	}

	unsigned char *image = new unsigned char[w*h*3];

	double exp2 = samples / w / h / exposure;

	auto bake = [&] (double v){
		return 255.0 * pow(v/exp2, 1.0/gamma);
	};

	auto c = [&] (double v) {
		return (unsigned char) std::max(0, std::min(255, (int) std::floor(bake(v))));
	};

	for(int i=0; i<w*h; ++i){
		image[i*3+2] = c(dat[i].r);
		image[i*3+1] = c(dat[i].g);
		image[i*3] = c(dat[i].b);
	}

	writeBMP(iname.c_str(), image, w, h);
}
