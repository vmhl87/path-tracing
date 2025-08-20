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
buffer _buffers[MAX_THREADS-1];

int main(int argc, char *argv[]){
	setup_scene();

	target.init();
	camera.t.init();
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

	std::thread threads[MAX_THREADS-1];

	for(int i=1; i<camera.threads; ++i)
		threads[i] = std::thread(render, i);

	render(0);

	for(int i=1; i<camera.threads; ++i)
		threads[i].join();

	for(int i=1; i<camera.threads; ++i)
		target.data += _buffers[i-1];

	target.write();
}

void render(int id){
	buffer &buf = id ? _buffers[id-1] : target.data;
	if(id) _buffers[id-1].init();

	auto start = std::chrono::high_resolution_clock::now();

	int samples = camera.c * camera.c;
	double factor = 1.0;

	while(samples > camera.w*camera.h*2)
		factor *= 2.0, samples /= 2;

	while(samples < camera.w*camera.h/2)
		factor /= 2.0, samples *= 2;

	for(int i=0; i<camera.spp; ++i){
		for(int x=0; x<camera.w; ++x)
			for(int y=0; y<camera.h; ++y){
				ray r; touch t;

				camera.get(x+rng::base(), y+rng::base(), r);

				for(int j=0; j<camera.bounces; ++j){
					if(hit(r, t)){
						r.c *= t.m -> c;

						if(t.m -> light){
							camera.set(buf, x, y, r.c);
							break;
						}

						t.scatter(r);

					}else{
						r.c *= sky(r.d) * (1.0 + 0.5*(rng::base()*2.0-1.0));
						camera.set(buf, x, y, r.c);
						break;
					}
				}
			}

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
						camera.set(buf, R.p, l.c(R.d)*E);
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
								camera.set(buf, R.p, r.c*E);
								++x;
							}
						};

						t.scatter(r);

					}else break;
				}
			}
		}

		if(camera.threads == 1 && camera.report && i%camera.report == 0)
			target.write((double) i / (double) camera.spp);
	}

	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = now - start;
	std::cout << "thread " << id+1 << " finished in "
		<< std::floor(duration.count()*1e3)/1e3 << "s\n";
}
