#pragma once
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

double transform, t2;

void setup_camera(){
	camera.w = 750;
	camera.h = 1000;
	camera.c = 750;

	camera.p = { 0, 1, -10 };
	camera.t.z = camera.p * -1;
	camera.t.y = { 0, 1, 0 };
	camera.t.init();
	camera.t.y -= camera.t.x * 0.1;

	camera.spp = 32;
	camera.sync = 8;
	camera.bounces = 8;

	// camera.spp = 1024;
}

color sky(vec d){
	vec sun_dir = (vec{2.6, 2.5, 7}).norm();
	color sun_color = vec{235, 207, 99} / 255.0;
	double haze_concentration = 10.0,
		   haze_falloff = 8.0,
		   sun_concentration = 90.0,
		   sun_luminance = 20.0,
		   haze_fraction = 0.8;

	double dir_frac = d.dot(sun_dir);
	double haze_raw = std::max(0.0, dir_frac*haze_concentration - haze_concentration + 1.0);
	double haze_proc = std::pow(haze_raw, haze_falloff);
	double sun_raw = dir_frac*sun_concentration - sun_concentration + 1.0;
	double sun_proc = std::pow(std::max(0.0, sun_raw), 2.0) * sun_luminance;
	double sun_total = std::max(haze_proc * haze_fraction, sun_proc);

	return vec{.31, .68, .91} * (0.5 - d.dot({0,1,0})*0.5) + sun_color * sun_total;
}

void branch(vec p1, vec p2, double w){
	vec r = rng::uniform2(), z = (p1-p2).norm();

	rects.push_back({
		.t = {
			.p = (p1+p2)/2.0,
			.y = r,
			.z = z,
		},
		.m = material::dielectric({.75, 2, .75}, .2, 10000),
		.w = w, .l = p1.dist(p2)/2.0 - .1,
	});

	rects.push_back({
		.t = {
			.p = (p1+p2)/2.0,
			.y = r.cross(z),
			.z = z,
		},
		.m = material::dielectric({1, .75, .35}, .2, 10000),
		.w = w/2, .l = p1.dist(p2)/2.0 - .2,
	});
}

void setup_scene(){
	// ground plane
	rects.push_back({
		.t = {
			.p = {0, -1.5, 0},
		},
		.m = material::dielectric({1, 1, 1}, 0.4, 5000),
		.w = 500, .l = 500,
	});

	// subject sphere
	spheres.push_back({
		.t = {
			.p = {},
		},
		.m = material::dielectric({0.05, 0.05, 0.125}, 0.1, 1000),
		.r = 1,
	});

	{
		//int seed = std::floor(rng::base()*10000);
		//rng::gen2.seed(seed); std::cout << "seed " << seed << '\n';
		// 8766
		// 5103
		// 7404
		rng::gen2.seed(7404);
	};

	// left plant
	{
		vec p = {2, -1.4, -1}, bp = {-6, 0, 4}, lp = p+bp;
		double f = M_PI / 3.0, r = bp.mag(), weight = 0.3;
		double phi = 0;
		int slices = 6;

		for(int i=1; i<=slices; ++i){
			phi += (slices*1.5-i) / slices * f/slices;
			vec np = p + bp * std::cos(phi) + vec{0, r, 0} * std::sin(phi);
			np += rng::uniform2() * 0.1 * (slices*0.5+i)/slices;
			branch(lp, np, (slices-i+1) * weight / slices);
			lp = np;
		}
	};

	// right plant
	{
		vec p = {-2, -1.4, 1.8}, bp = {6, 0, -4}, lp = p+bp;
		double f = M_PI / 3.0, r = bp.mag(), weight = 0.3;
		double phi = 0;
		int slices = 6;

		for(int i=1; i<=slices; ++i){
			phi += (slices*1.5-i) / slices * f/slices;
			vec np = p + bp * std::cos(phi) + vec{0, r, 0} * std::sin(phi);
			np += rng::uniform2() * 0.1 * (slices*0.5+i)/slices;
			branch(lp, np, (slices-i+1) * weight / slices);
			lp = np;
		}
	};

	// left mountain
	{
		vec p = { -50, -2, 200 },
			y = (camera.p-p).norm(),
			x = {0, 1, 0},
			z = x.cross(y) + x;

		rects.push_back({
			.t = {
				.p = p,
				.y = y,
				.z = z,
			},
			.m = material::diffuse({.75, .75, .75}),
			.w = 30, .l = 30,
		});
	};

	// right mountain
	{
		vec p = { -10, -2, 150 },
			y = (camera.p-p).norm(),
			x = {0, 1, 0},
			z = x.cross(y) + x;

		rects.push_back({
			.t = {
				.p = p,
				.y = y,
				.z = z,
			},
			.m = material::diffuse({.5, .5, .6}),
			.w = 15, .l = 15,
		});
	};
}
