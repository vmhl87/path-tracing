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
	setup_camera();
	camera.t.init();
	target.init();

	setup_scene();
	for(sphere &s : spheres) s.init();
	for(rect &s : rects) s.init();

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

void backward_trace(buffer &buf, int x, int y);
void forward_trace(buffer &buf, const light &l);

void render(int id){
	buffer &buf = id ? _buffers[id-1] : target.data;
	if(id) _buffers[id-1].init();

	auto start = std::chrono::high_resolution_clock::now();

	bool use_global = sky({0, 0, 0}).x > -1;

	if(camera.threads == 1 && camera.sync){
		for(int i=0; i<camera.spp; ++i){
			if(use_global)
				for(int x=0; x<camera.w; ++x)
					for(int y=0; y<camera.h; ++y)
						backward_trace(buf, x, y);

			for(light &l : lights)
				forward_trace(buf, l);

			if(i%camera.sync == 0)
				target.write((double) (i+1) / (double) camera.spp);
		}
	}else{
		if(use_global)
			for(int x=0; x<camera.w; ++x)
				for(int y=0; y<camera.h; ++y)
					for(int i=0; i<camera.spp; ++i)
						backward_trace(buf, x, y);

		for(light &l : lights)
			for(int x=0; x<camera.spp; ++x)
				forward_trace(buf, l);
	}

	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = now - start;
	std::cout << "thread " << id+1 << " finished in "
		<< std::floor(duration.count()*1e3)/1e3 << "s\n";
}

void backward_trace(buffer &buf, int x, int y){
	ray r; touch t;
	bool smooth = 1;

	camera.get(x+rng::base(), y+rng::base(), r);

	for(int j=0; j<camera.bounces; ++j){
		if(hit(r, t, 1)){
			r.c *= t.m -> c;
			if(t.m -> smooth < 200) smooth = 0;

			if(t.m -> light){
				camera.set(buf, x, y, r.c);
				break;
			}

			t.scatter(r);

		}else{
			r.c *= sky(r.d) * (smooth ? 1 : 0.5 + rng::base());
			camera.set(buf, x, y, r.c);
			break;
		}
	}
}

void forward_trace(buffer &buf, const light &l){
	int samples = camera.w * camera.h;
	double factor = camera.c * camera.c / camera.w / camera.h;

	for(int x=0; x<samples; ++x){
		ray r; touch t; l.get(r);

		ray R; touch T;
		R.p = r.p; R.d = (camera.p-r.p).norm();
		if(!hit(R, T) || T.d*T.d > camera.p.distsq(R.p)){
			double E = factor/2.0/camera.p.distsq(R.p);
			camera.set(buf, R.p, l.c(R.d)*E);
		}

		for(int j=0; j<camera.bounces; ++j){
			if(hit(r, t)){
				r.c *= t.m -> c;

				ray R; touch T;
				R.p = t.p; R.d = (camera.p-t.p).norm();
				if(!hit(R, T) || T.d*T.d > camera.p.distsq(R.p)){
					double E = factor*t.scatter(r.d, R.d)/camera.p.distsq(R.p);
					camera.set(buf, R.p, r.c*E);
					++x;
				}

				t.scatter(r);

			}else break;
		}
	}
}
