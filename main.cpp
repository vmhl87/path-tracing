#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#define MAX_THREADS 12

#include "vec.cpp"
#include "rng.cpp"
#include "ray.cpp"
#include "camera.cpp"

#include "light.cpp"
#include "surface.cpp"

#include "scene.cpp"

void render(int id);

int main(int argc, char *argv[]){
	rng::init();

	setup_scene();

	camera.init();
	for(rect &s : rects) s.t.init();

	if(argc > 1){
		try{
			int threads = std::stoi(argv[1]);
			if(threads > 1 && threads <= MAX_THREADS)
				camera.threads = threads;

		}catch(const std::invalid_argument &e){
			std::cerr << "invalid arg " << e.what() << '\n';
		}catch(const std::out_of_range &e){
			std::cerr << "out of range " << e.what() << '\n';
		}
	}

	std::cout << "tracing with " << camera.threads << " threads\n";

	auto start = std::chrono::high_resolution_clock::now();

	if(camera.threads == 1) render(0);
	else{
		std::thread threads[MAX_THREADS];

		for(int i=0; i<camera.threads; ++i)
			threads[i] = std::thread(render, i);

		for(int i=0; i<camera.threads; ++i)
			threads[i].join();
	}

	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = now - start;
	std::cout << "render " << std::floor(duration.count()*1e3)/1e3 << "s\n";

	camera.write();
}

void render(int id){
	camera.init(id);

	for(int i=0; i<camera.spp; ++i){
		for(int x=0; x<camera.w; ++x)
			for(int y=0; y<camera.h; ++y){
				ray r; touch t;
				camera.get(x+rng::base(), y+rng::base(), r);

				for(int j=0; j<camera.bounces; ++j){
					if(hit(r, t)){
						r.c *= t.m -> c;

						if(t.m -> light){
							camera.set(id, x, y, r.c);
							break;
						}

						t.scatter(r);

					}else{
						r.c *= sky(r.d) * (1.0 + 0.5*(rng::base()*2.0-1.0));
						camera.set(id, x, y, r.c);
						break;
					}
				}
			}

		int samples = camera.c * camera.c;
		double factor = 1.0;

		while(samples > camera.w*camera.h*2)
			factor *= 2.0, samples /= 2;

		while(samples < camera.w*camera.h/2)
			factor /= 2.0, samples *= 2;

		for(light &l : lights){
			for(int x=0; x<samples; ++x){
				ray r; touch t; l.get(r);

				double d = 0;

				{
					ray R; touch T;
					R.p = r.p; R.d = (camera.p-r.p).norm();
					if(!hit(R, T) || T.d > camera.p.dist(R.p)){
						double E = camera.p.dist(R.p);
						E = factor/2.0/E/E;
						camera.set(id, R.p, l.c(R.d)*E);
					}
				};

				for(int j=0; j<camera.bounces; ++j){
					if(hit(r, t)){
						r.c *= t.m -> c;
						d += r.p.dist(t.p);

						{
							ray R; touch T;
							R.p = t.p; R.d = (camera.p-t.p).norm();
							if(!hit(R, T) || T.d > camera.p.dist(R.p)){
								//double E = camera.p.dist(R.p)+d;
								double E = camera.p.dist(R.p);
								E = factor*t.scatter(r.d, R.d)/E/E;
								camera.set(id, R.p, r.c*E);
								++x;
							}
						};

						t.scatter(r);

					}else break;
				}
			}
		}

		if(camera.threads == 1 && camera.report && i%camera.report == 0)
			camera.write((double) i / (double) camera.spp);
	}
}
