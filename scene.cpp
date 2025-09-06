#pragma once
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

double transform, t2;

void setup_camera(){
	camera.w = 600;
	camera.h = 400;
	camera.c = 600;

	camera.p = { 0, 0, -10 };
	camera.t.init();
	
	camera.spp = 128;
	camera.sync = 32;
	camera.bounces = 5;
}

color sky(vec d){
	return vec{.1,.1,.1};
	return vec{1, 1, 1};
}

void setup_scene(){
	double F = 0;

	for(int i=0; i<camera.time*camera.fps; ++i){
		if(F < 1.0) F += 0.1;
		else if(F < 5) F += 1;
		else if(F < 30) F += 5;
		else if(F < 100) F += 20;
		else F += 200;
	}

	// ground
	rects.push_back({
		.t = {
			.p = { 0, -1, 0 },
			.y = { 0, 1, 0 },
			.z = { 0, 0, 1 },
		},
		.m = material::metal({.75, .75, .75}, F),
		//.m = material::diffuse({.75, .75, .75}),
		//.m = material::dielectric({.75, .75, .75}, 0.5, 1000),
		.w = 100, .l = 100,
	});

	// lights
	
	spheres.push_back({
		.t = {
			.p = {-2, 0, 0},
		},
		.m = material::light({17, .68, .68}),
		.r = .25,
	});

	lights.push_back({
		.get = [] (ray &r) {
			r.c = {5, .2, .2};
			r.p = rng::uniform() * .25;
			r.d = (r.p.norm()+rng::uniform()).norm();
			r.p.x += 2;
		},
		.c = [] (vec&) { return color{2.5, .1, .1}; },
	});
}
