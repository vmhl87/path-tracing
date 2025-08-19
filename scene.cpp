#pragma once

color sky(vec &d){
	return (vec){1,1,1} * (d.dot((vec){0,1,0})*0.5+0.5);
}

void setup_scene(){
	camera.w = 600;
	camera.h = 600;
	camera.c = 400;

	camera.p = { 0.2, 0, -3 };
	camera.t.z = { -0.1, 0, 1 };
	camera.t.y = { -0.1, 1, 0 };

	camera.spp = 16;
	camera.report = 2;
	camera.bounces = 5;

	camera.spp = 128;
	camera.bounces = 10;

	double L = 0.25;
	L = 0.75;

	// floor
	rects.push_back({
		.t = {
			.p = {0, -1, 0},
			.y = {0, 1, 0},
			.z = {0, 0, 1},
		},
		.m = {
			.c = {0.75, 0.75, 0.75},
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
			.c = {0.75, 0.75, 0.75},
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
			//.c = {.7, .7, .7},
			.c = {.9, .9, .9},
			.smooth = 25,
		},
		.r = 0.3,
	});

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

	/*
	double m = 300;
	vec d = {0, -1, 0};

	lights.push_back({
		.get = [m, d] (ray &r) {
			r.p = {
				0.7 * (rng::base()*2.0-1.0),
				0.95,
				0.35 * (rng::base()*2.0-1.0),
			};

			if(rng::base() < 0.25) r.d = rng::uniform();
			else r.d = cosine_lobe(m, d);

			r.c = {1,1,1};
		},
		.c = [m, d] (vec &v) {
			return (color) {1,1,1} * (
				0.25 +
				0.75 * cosine_dist(m, d, v)
			);
		},
	});
	*/
}
