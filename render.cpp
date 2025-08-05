#include <iostream>
#include <string>
#include <cmath>

#include "headers.hpp"

int main(){
	img image(256, 256);

	for(int i=0; i<256; ++i)
		for(int j=0; j<256; ++j)
			image.at(i, j) = {i/256.0, j/256.0, 0.5};

	image.write("image.bmp");
}
