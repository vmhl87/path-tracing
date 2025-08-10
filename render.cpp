#include <random>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "trace.cpp"

bool whitespace(std::string s){
	for(char c : s)
		if(c != ' ' && c != '\t')
			return false;

	return true;
}

bool comment(std::string s){
	for(char c : s)
		if(c == '#') return true;
		else if(c != ' ' && c != '\t')
			return false;

	return false;
}

int count_tabs(std::string s){
	int res = 0;

	for(char c : s)
		if(c == '\t') ++res;
		else break;

	return res;
}

struct node{
	std::string s;
	std::vector<node*> child;
	bool root;

	node(std::string S, int r){
		root = r == 0;
		s = S;
	}

#define read(type) type read_##type(){ \
	std::istringstream v(s); \
	std::string S; v >> S; \
	type res;

#define end ; return res; }

	read(int) v >> res end

	read(double) v >> res end

	read(int64_t) v >> res end

	read(vec) v >> res.x >> res.y >> res.z end

#undef read
#undef end

	std::string read_str(){
		std::istringstream v(s);
		std::string S; v >> S;
		std::string x; v >> x;
		return x;
	}

};

#define handle(type) type handle_##type(node &n){ \
	type res = {}; \
	for(node *C : n.child){ \
		node &c = *C; \
		std::istringstream V(c.s); \
		std::string S; V >> S; \
		if(0){

#define end }else std::cout << "unused option: " << c.s << '\n'; } return res; }

#define case(x) }else if(S == #x){ res. x =

handle(dir)
	case(f) c.read_vec().norm();
	case(u) c.read_vec().norm();
end;

handle(cam)
	case(p) c.read_vec();
	case(d) handle_dir(c);
	case(w) c.read_int();
	case(h) c.read_int();
	case(c) c.read_double();

	case(iter) c.read_int64_t();
	case(report) c.read_int64_t();
	case(chunk) c.read_int64_t();

	case(exposure) c.read_double();
	case(gamma) c.read_double();
	case(blur) c.read_double(); res.use_blur = 1;

	case(bounces) c.read_int();

	case(adjust) c.read_int();

	case(iname) c.read_str(); res.raw_output = 0;
	case(rname) c.read_str(); res.raw_output = 1;
end;

handle(noisy_vec)
	case(p) c.read_vec();  // deprecated
	case(r) c.read_double();
	case(t) c.read_int();
end;

handle(light)
	case(p) handle_noisy_vec(c); res.p.p = c.read_vec();
	case(d) handle_noisy_vec(c); res.d.p = c.read_vec();
	case(c) c.read_vec();
	case(visible) 1;
end;

handle(material)
	case(c) c.read_vec();  // deprecated
	case(shine) c.read_double();
	case(gloss) c.read_double();
end;

bool use_motion_blur = false;

handle(sphere)
	case(p) c.read_vec();
	case(d) c.read_vec(); res.blur = true; use_motion_blur = true;
	case(r) c.read_double();
	case(mat) handle_material(c); res.mat.c = c.read_vec();
end;

#undef handle
#undef case
#undef end

#define case(x) }else if(S == #x){

void handle_global(node &c){
	for(node *C : c.child){
		node &c = *C;
		std::istringstream V(c.s);
		std::string S; V >> S;
		if(S == ""){
			case(d) global_mag = (global_dir = c.read_vec()).mag()/2.0;
		}else std::cout << "unused option: " << c.s << '\n';
	}
}

void handle(node &c){
	std::istringstream V(c.s);
	std::string S; V >> S;
	if(S == ""){
		case(camera) camera = handle_cam(c);
		case(sphere) spheres.push_back(handle_sphere(c));
		case(light) lights.push_back(handle_light(c));
		case(global) global = c.read_vec(); handle_global(c); use_global = true;
	}else std::cout << "unused option: " << c.s << '\n';
}

#undef case

int main(){
	std::string s;
	int level = 0, comment_level = 1000;

	std::vector<node*> nodes;
	std::vector<int> ctx;

	while(true){
		std::getline(std::cin, s);
		if(s == "done") break;

		if(whitespace(s)) continue;

		int lvl = count_tabs(s);

		if(lvl > comment_level) continue;

		if(comment(s)){
			comment_level = lvl;
			continue;
		}else comment_level = 1000;

		if(lvl > level) ctx.push_back(nodes.size()-1);
		while(lvl < (int) ctx.size()) ctx.pop_back();
		level = lvl;

		node *x = new node(s, ctx.size());
		if(ctx.size()) nodes[ctx.back()]->child.push_back(x);
		nodes.push_back(x);
	}

	for(node *n : nodes) if(n->root) handle(*n);

	rng::init();
	camera.init();

	for(int64_t x=0; x<camera.iter; ++x){
		for(const light &l : lights){
			for(int64_t i=0; i<camera.chunk; ++i){
				ray r; l(r); if(use_motion_blur) r.t = rng::norm();

				if(l.visible){
					ray R;
					R.p = r.p;
					R.d = (camera.p-r.p).norm();
					R.c = r.c;
					add(R);
				}

				//forward_trace(r, camera.bounces);

				/*
				{
					int x_coord = std::floor(rng::base()*camera.w),
						y_coord = std::floor(rng::base()*camera.h);

					ray r2;

					r2.c = global, r2.p = camera.p; r2.t = r.t;
					r2.d = camera.d.project({
						x_coord-camera.w/2 + rng::base() + (camera.use_blur ? rng::norm()*camera.blur : 0),
						camera.h/2-y_coord + rng::base() + (camera.use_blur ? rng::norm()*camera.blur : 0),
						camera.c,
					}).norm();

					touch t = hit(r2);

					if(t.hit){
						forward_trace(r, t.p, r2.d, t.norm, t.mat.c, t.mat.shine, x_coord, y_coord, camera.bounces);

					}else forward_trace(r, camera.bounces);
				};
				*/

				{
					int x_coord = std::floor(rng::base()*camera.w),
						y_coord = std::floor(rng::base()*camera.h);

					ray r2;
					r2.c = {1, 1, 1}, r2.p = camera.p; r2.t = r.t;
					r2.d = camera.d.project({
						x_coord-camera.w/2 + rng::base() + (camera.use_blur ? rng::norm()*camera.blur : 0),
						camera.h/2-y_coord + rng::base() + (camera.use_blur ? rng::norm()*camera.blur : 0),
						camera.c,
					}).norm();

					// vars (before goto)
					touch t1, t2, t3;
					double d;
					vec energy;

					// hit first surface
					t1 = hit(r2);
					if(!t1.hit || t1.mat.shine < 50.1) goto done;

					// scatter once
					ray r3;
					r3.p = t1.p; r3.t = r2.t; r3.d = r2.d;
					while(r3.d.dot(t1.norm) < 0.0) r3.d = scatter(r3.d, t1.norm, t1.mat.shine);

					// hit second surface
					t2 = hit(r3);
					if(!t2.hit || t2.mat.shine > 50.1) goto done;

					// orient to light
					ray r4;
					r4.p = t2.p; r4.d = (l.p.p-t2.p).norm();

					// see if we reach light
					t3 = hit(r4);
					if(t3.hit && t3.d < l.p.p.dist(t2.p)) goto done;

					// normalize by probability distribution
					//d = (camera.p.distsq(t1.p)) / (t1.p.distsq(t2.p) + t2.p.distsq(l.p.p)) / M_PI / 4.0;
					//d /= 64.0;
					//d = 1.0;
					//d = (camera.p.distsq(t1.p) + t1.p.distsq(t2.p)) / (t2.p.distsq(l.p.p)) / M_PI / 4.0;
					//d = 1.0 / (t2.p.distsq(l.p.p)) / M_PI / 4.0;
					//d = 1.0 / (t2.p.distsq(l.p.p));
					//d = 8.0 * t2.norm.dot(r4.d) / (t2.p.distsq(l.p.p));
					d = 1.0 / (t2.p.distsq(l.p.p));
					energy = t1.mat.c * t2.mat.c * l.c * scatter(r3.d, r4.d, t2.norm, t2.mat.shine) * d;
					//energy = t1.mat.c * t2.mat.c * l.c * d;

					camera.add(x_coord, y_coord, energy);
					r.c = {0, 0, 0};

done:
					forward_trace(r);
				};
			}
		}

		if(use_global) for(int64_t i=0; i<camera.chunk; ++i){
			// load balanced
			int x_coord, y_coord; camera.pick(x_coord, y_coord);

			ray r;

			r.c = global, r.p = camera.p; if(use_motion_blur) r.t = rng::norm();
			r.d = camera.d.project({
				x_coord-camera.w/2 + rng::base() + (camera.use_blur ? rng::norm()*camera.blur : 0),
				camera.h/2-y_coord + rng::base() + (camera.use_blur ? rng::norm()*camera.blur : 0),
				camera.c,
			}).norm();

			if(backward_trace(r)) camera.add(x_coord, y_coord, r.c);
		}

		if(camera.report && x%camera.report == 0)
			camera.write(false, camera.adjust ? (double) x / camera.iter : 1.0);
	}

	camera.write(true, 1.0);
}
