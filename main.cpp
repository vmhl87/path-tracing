#include <iostream>

#include "vec.cpp"
#include "rng.cpp"
#include "ray.cpp"
#include "camera.cpp"

//#include "light.cpp"
#include "surface.cpp"

bool fancy = 1;

color sky(vec &d){
	return (vec){1,1,1} * (d.dot((vec){0,1,0})*0.5+0.5);
}

int main(){
	rng::init();

	camera.w = 600;
	camera.h = 600;
	camera.c = 400;

	camera.p = { 0.2, 0, -3 };
	camera.t.z = { -0.1, 0, 1 };
	camera.t.y = { -0.1, 1, 0 };
	camera.init();

	camera.spp = 64;
	camera.report = 8;
	camera.bounces = 5;

	if(fancy){
		camera.spp = 256;
		camera.report = 0;
		camera.bounces = 10;
	}

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
			//.smooth = 30,
			//.smooth = 10,
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
			.c = {.2, .2, .9},
			//.smooth = 30,
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
			.c = {1, 0, 0},
			//.smooth = 30,
			.smooth = 300,
		},
		.w = 1, .l = L,
	});

	// invis wall
	/*
	rects.push_back({
		.t = {
			.p = {0, 0, -1},
			.y = {0, 0, 1},
			.z = {0, 1, 0},
		},
		.m = {
			.c = {.7, .7, .7},
		},
		.w = 1, .l = L,
		//.bias = true,
	});
	*/

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
			.smooth = 5,
		},
		.r = 0.3,
	});

	for(rect &r : rects) r.t.init();

	for(int i=0; i<camera.spp; ++i){
		// global illumination
		for(int x=0; x<camera.w; ++x)
			for(int y=0; y<camera.h; ++y){
				ray r; touch t;
				camera.get(x+rng::base(), y+rng::base(), r);

				for(int j=0; j<camera.bounces; ++j){
					if(hit(r, t)){
						r.c *= t.m -> c;
						t.scatter(r);

					}else{
						r.c *= sky(r.d);
						camera.set(x, y, r.c);
						break;
					}
				}
			}

		// lighting

		int samples = camera.w * camera.h;

		for(int x=0; x<samples; ++x){
			ray r; touch t;
			r.p = {0.7 * (rng::base()*2.0-1.0), 0.99, 0.35 * (rng::base()*2.0-1.0)};
			//r.p = {0.1 * (rng::base()*2.0-1.0), 0.99, 0.35 * (rng::base()*2.0-1.0)};
			r.d = rng::uniform();
			r.c = {1, 1, 1};

			//r.c /= 2.0;

			auto sq = [] (double x) { return x*x; };
			double d = 0;

			{
				ray R; touch T;
				R.p = r.p; R.d = (camera.p-r.p).norm();
				if(!hit(R, T) || T.d > camera.p.dist(R.p))
					camera.set(R.p, r.c/2.0/sq(camera.p.dist(R.p)));
			};

			for(int j=0; j<camera.bounces; ++j){
				if(hit(r, t)){
					r.c *= t.m -> c;
					d += r.p.dist(t.p);

					{
						ray R; touch T;
						R.p = t.p; R.d = (camera.p-t.p).norm();
						R.c = r.c * t.scatter(r.d, R.d);
						if(!hit(R, T) || T.d > camera.p.dist(R.p))
							//camera.set(R.p, R.c/2.0/sq(camera.p.dist(R.p)+d));
							camera.set(R.p, R.c/sq(camera.p.dist(R.p)+d)), ++x;
					};

					//r.c *= 0.5;
					r.c *= 0.75;
					t.scatter(r);

				}else break;
			}
		}

		if(camera.report && i%camera.report == 0)
			camera.write((double) i / (double) camera.spp);
	}

	camera.write();
}
