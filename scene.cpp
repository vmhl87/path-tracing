#pragma once
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

double transform, t2;

void setup_camera(){
	camera.w = 400;
	camera.h = 400;
	camera.c = 600;

	camera.p = { 0, 0, -10 };
	camera.t.init();
	
	camera.spp = 128;
	camera.sync = 32;
	camera.bounces = 5;

	camera.spp = 1024;
}

color sky(vec d){
	return vec{.1,.1,.1};
	return vec{1, 1, 1};
}

void setup_scene(){
	// ground
	rects.push_back({
		.t = {
			.p = { 0, -1, 0 },
			.y = { 0, 1, 0 },
			.z = { 0, 0, 1 },
		},
		.m = material::metal({.75, .75, .75}, 0),
		//.m = material::diffuse({.75, .75, .75}),
		//.m = material::dielectric({.75, .75, .75}, 0.5, 1000),
		.w = 100, .l = 100,
	});

	// light setups
	
	bool T = 1;

	if(T) spheres.push_back({
		.t = {
			.p = {},
		},
		.m = material::light({20, .8, .8}),
		.r = .25,
	});

	if(!T) lights.push_back({
		.get = [] (ray &r) {
			r.c = {5, .2, .2};
			r.p = rng::uniform() * .25;
			r.d = (r.p.norm()+rng::uniform()).norm();
		},
		.c = [] (vec&) { return color{2.5, .1, .1}; },
	});
}
