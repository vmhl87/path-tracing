#pragma once
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

double transform, t2;

void setup_camera(){
	camera.w = 600;
	camera.h = 600;
	camera.c = 400;

	camera.p = { 0.2, 0, -3 };
	camera.t.z = { -0.1, 0, 1 };
	camera.t.y = { -0.1, 1, 0 };

	// animate
	transform = std::sin(std::min(2.75,camera.time)*M_PI/2.0/2.75);
	t2 = std::min(1.0, 4.0-camera.time);
	camera.p = { 0.2 * (1.1 - transform)/1.1 - 1.2*(1.0-t2), 0, -3 };
	camera.t.y = { -0.1 * (1.0 - transform), 1, 0 };
	camera.t.z = { -0.1 * (1.1 - transform)/1.1 + 0.35*(1.0-t2), 0, 1 };
	camera.p -= camera.t.z * transform * 0.5;
	camera.w *= 1.5, camera.c *= 1.5;
	camera.c *= 1.0 + 1.5*(1.0-t2);

	camera.spp = 16;
	camera.sync = 2;
	camera.bounces = 5;

	camera.spp = 64;
	camera.sync = 8;
}

color sky(vec d){
	//return vec{-2,0,0};
	return vec{1,1,1} * (d.dot({0,-0.5+1.5*t2,0})*0.5+0.5);
	return vec{1,1,1} * (d.dot({0,1,0})*0.5+0.5);
}

void setup_scene(){
	double L = 0.75;

	L = 0.1 + 0.65*t2;
	double L2 = 0.1 + 0.9*t2;
	double t3 = (1.0-t2) * M_PI / 2.0;

	// floor
	rects.push_back({
		.t = {
			.p = {0, -1, 0},
			.y = {0, 1, 0},
			.z = {-std::sin(t3), 0, std::cos(t3)},
		},
		//.m = material::diffuse({1, 1, 1}),
		.m = material::diffuse({.95, .95, .95}),
		.w = L2, .l = L2,
	});

	// ceiling
	rects.push_back({
		.t = {
			.p = {0, 1, 0},
			.y = {0, -1, 0},
			.z = {std::sin(t3), 0, std::cos(t3)},
		},
		//.m = material::diffuse({1, 1, 1}),
		.m = material::diffuse({.95, .95, .95}),
		.w = L2, .l = L2,
	});

	// left wall
	rects.push_back({
		.t = {
			.p = {-1, 0, 0},
			.y = {1, 0, 0},
			.z = {0, std::cos(t3), std::sin(t3)},
		},
		.m = material::dielectric({0.1, 0.75, 0.1}, 0.1, 300),
		.w = L2, .l = L,
	});

	// back wall
	rects.push_back({
		.t = {
			.p = {0, 0, 1},
			.y = {0, 0, -1},
			.z = {std::sin(t3), std::cos(t3), 0},
		},
		.m = material::metal({0.2, 0.2, 0.75}, 500),
		.w = L2, .l = L,
	});

	// right wall
	rects.push_back({
		.t = {
			.p = {1, 0, 0},
			.y = {-1, 0, 0},
			.z = {0, std::cos(t3), -std::sin(t3)},
		},
		.m = material::diffuse({0.75, 0.1, 0.1}),
		.w = L2, .l = L,
	});

	// smoothed metal ball
	spheres.push_back({
		.t = {
			.p = {-0.35, -0.75, 0.2},
		},
		.m = material::metal({.9, .9, .9}, 200),
		.r = 0.3*L2,
	});

	// black diffuse ball
	spheres.push_back({
		.t = {
			.p = {0.35, -0.75, 0.2},
		},
		.m = material::diffuse({.1, .1, .1}),
		.r = 0.3*L2,
	});

	// ceiling light - 2 styles

	/*
	rects.push_back({
		.t = {
			.p = {0, .9, 0},
			.y = {0, -1, 0},
		},
		.m = material::light({1, 1, 1}),
		.w = 0.7, .l = 0.35,
	});
	*/

	double V = L2*(1.1-transform)/1.1;
	simple_light({
		0.1,
		0.9 - 0.2 * transform,
		0.1,
	}, {V,V,V});

	/*
	double V = L2*(1.1-transform)/1.1;
	simple_light({
		0.7 * (rng::base()*2.0-1.0) * (1.05 - transform) / 1.05,
		0.9 - 0.2 * transform,
		0.35 * (rng::base()*2.0-1.0) * (1.1 - transform) / 1.1,
	}, {V,V,V});
	*/
}
