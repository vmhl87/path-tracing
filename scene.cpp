#pragma once
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

void setup_camera(){
	camera.w = 600;
	camera.h = 600;
	camera.c = 400;

	camera.p = { 0.2, 0, -3 };
	camera.t.z = { -0.1, 0, 1 };
	camera.t.y = { -0.1, 1, 0 };

	camera.spp = 16;
	camera.sync = 2;
	camera.bounces = 5;
}

color sky(vec d){
	//return vec{-2, 0, 0};
	return vec{1,1,1} * (d.dot({0,1,0})*0.5+0.5);
}

void setup_scene(){
	double L = 0.75;

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
			.c = {0, 0.75, 0},
			.smooth = 10,
		},
		.w = 1, .l = L,
	});

	// back wall
	rects.push_back({
		.t = {
			.p = {0, 0, 1},
			.y = {0, 0, -1},
			.z = {0, 1, 0},
		},
		.m = {
			.c = {0.2, 0.2, 0.75},
			//.smooth = 300,
			.smooth = 1000,
		},
		.w = 1, .l = L,
	});

	// right wall
	rects.push_back({
		.t = {
			.p = {1, 0, 0},
			.y = {-1, 0, 0},
			.z = {0, 1, 0},
		},
		.m = {
			.c = {0.75, 0, 0},
			//.smooth = 10,
		},
		.w = 1, .l = L,
	});

	// smooth metal ball
	spheres.push_back({
		.t = {
			.p = {-0.35, -0.75, 0.2},
		},
		.m = {
			.c = {.9, .9, .9},
			.smooth = 300,
		},
		.r = 0.3,
	});

	// rough metal ball
	spheres.push_back({
		.t = {
			.p = {0.35, -0.75, 0.2},
		},
		.m = {
			.c = {.9, .9, .9},
			.smooth = 25,
		},
		.r = 0.3,
	});

	// ceiling light - 2 styles

	/*
	rects.push_back({
		.t = {
			.p = {0, .99, 0},
			.y = {0, -1, 0},
		},
		.m = {
			.light = true,
			.c = {2, 2, 2},
		},
		.w = 0.7, .l = 0.35,
	});
	*/

	simple_light({
		0.7 * (rng::base()*2.0-1.0),
		0.99,
		0.35 * (rng::base()*2.0-1.0),
	}, {1,1,1});
}
