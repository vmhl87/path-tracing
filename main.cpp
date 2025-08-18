#include <iostream>

#include "util.cpp"

bool fancy = 1;

color sky(vec &d){
	return {0, 0, 0};
	return {1, 1, 1};
}

int main(){
	rng::init();

	camera.w = 600;
	camera.h = 600;
	camera.c = 600;

	camera.p = { 0, 0, -3 };
	camera.t.z = { 0, 0, 1 };
	camera.t.y = { 0, 1, 0 };

	camera.spp = 16;
	camera.report = 1;
	camera.bounces = 5;

	if(fancy){
		camera.spp = 256;
		camera.report = 0;
		camera.bounces = 10;
	}

	// light
	rects.push_back({
		.t = {
			.p = {0, 0.99, 0},
			.y = {0, 1, 0},
			.z = {0, 0, 1},
		},
		.m = {
			.light = true,
			//.c = {1, 1, 1},
			.c = {1.5, 1.5, 1.5},
		},
		.w = 0.7, .l = 0.35,
		//.bias = true,
	});

	// back wall
	rects.push_back({
		.t = {
			.p = {0, 0, 1},
			.y = {0, 0, -1},
			.z = {0, 1, 0},
		},
		.m = {
			.c = {.2, .2, .9},
			.specular = 0.25,
			.gloss = 0.05,
		},
		.w = 1, .l = 1,
	});

	// floor
	rects.push_back({
		.t = {
			.p = {0, -1, 0},
			.y = {0, 1, 0},
			.z = {0, 0, 1},
		},
		.m = {
			.c = {1, 1, 1},
		},
		.w = 1, .l = 1,
	});

	// ceiling
	rects.push_back({
		.t = {
			.p = {0, 1, 0},
			.y = {0, -1, 0},
			.z = {0, 0, 1},
		},
		.m = {
			.c = {1, 1, 1},
		},
		.w = 1, .l = 1,
	});

	// left wall
	rects.push_back({
		.t = {
			.p = {-1, 0, 0},
			.y = {1, 0, 0},
			.z = {0, 1, 0},
		},
		.m = {
			.c = {0, 1, 0},
		},
		.w = 1, .l = 1,
	});

	// right wall
	rects.push_back({
		.t = {
			.p = {1, 0, 0},
			.y = {-1, 0, 0},
			.z = {0, 1, 0},
		},
		.m = {
			.c = {1, 0, 0},
		},
		.w = 1, .l = 1,
	});

	// invis wall
	rects.push_back({
		.t = {
			.p = {0, 0, -1},
			.y = {0, 0, 1},
			.z = {0, 1, 0},
		},
		.m = {
			.c = {.7, .7, .7},
		},
		.w = 1, .l = 1,
		.bias = true,
	});

	// smooth metal ball
	spheres.push_back({
		.t = {
			.p = {-0.35, -0.75, 0.2},
		},
		.m = {
			.c = {.7, .7, .7},
			.specular = 1,
		},
		.r = 0.3,
	});

	// rough metal ball
	spheres.push_back({
		.t = {
			.p = {0.35, -0.75, 0.2},
		},
		.m = {
			.c = {1, 1, 1},
			.specular = 1,
			.gloss = 0.5,
		},
		.r = 0.3,
	});

	// patches of light
	
	rects.push_back({
		.t = {
			.p = {-0.3, -0.95, -0.7},
		},
		.m = {
			.light = true,
			.c = {0.5, 0, 0},
		},
		.w = .05, .l = .05,
	});

	rects.push_back({
		.t = {
			.p = {0, -0.9, -0.5},
		},
		.m = {
			.light = true,
			.c = {0, 0, 0.5},
		},
		.w = .05, .l = .05,
	});

	rects.push_back({
		.t = {
			.p = {0.3, -0.95, -0.7},
		},
		.m = {
			.light = true,
			.c = {0, 0.5, 0},
		},
		.w = .05, .l = .05,
	});

	camera.init();

	for(rect &r : rects) r.t.init();

	for(int i=0; i<camera.spp; ++i){
		for(int x=0; x<camera.w; ++x)
			for(int y=0; y<camera.h; ++y){
				ray r; touch t;
				camera.get(x+rng::base(), y+rng::base(), r);
				bool light = 0;

				for(int j=0; j<camera.bounces; ++j){
					if(hit(r, t)){
						r.c *= t.m -> c;

						if(t.m->light){
							light = 1;
							break;
						}

						t.scatter(r);

					}else{
						r.c *= sky(r.d);
						light = 1;
						break;
					}
				}

				if(light) camera.set(x, y, r.c);
			}

		if(camera.report && i%camera.report == 0)
			camera.write((double) i / (double) camera.spp);
	}

	camera.write();
}
