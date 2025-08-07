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
		if(c != ' ' && c != '\t')
			return false;
		else if(c == '#')
			return true;

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

};

#define handle(type) type handle_##type(node &n){ \
	type res = {}; \
	for(node *C : n.child){ \
		node &c = *C; \
		std::istringstream V(c.s); \
		std::string S; V >> S; \
		if(0){

#define end }} return res; }

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
	case(exposure) c.read_double();
	case(gamma) c.read_double();
	case(bounces) c.read_int();
	case(adjust) c.read_int();
end;

handle(noisy_vec)
	case(p) c.read_vec();
	case(r) c.read_double();
	case(t) c.read_int();
end;

handle(light)
	case(p) handle_noisy_vec(c);
	case(d) handle_noisy_vec(c);
	case(c) c.read_vec();
	case(granular) c.read_int();
end;

handle(material)
	case(c) c.read_vec();
end;

handle(sphere)
	case(p) c.read_vec();
	case(r) c.read_double();
	case(mat) handle_material(c);
end;

#undef handle
#undef case
#undef end

#define case(x) }else if(S == #x){

void handle(node &c){
	std::istringstream V(c.s);
	std::string S; V >> S;
	if(S == ""){
		case(camera) camera = handle_cam(c);
		case(sphere) spheres.push_back(handle_sphere(c));
		case(light) lights.push_back(handle_light(c));
		case(global) global = c.read_vec();
	}
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

	// backward tracing
	
	camera.init();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<double> d(0.0, 1.0);

	bool use_global = global.mag() > 1e-18;
	
	for(int64_t x=0; x<camera.iter; ++x){
		for(const light &l : lights){
			if(l.granular > 1 && x%l.granular) continue;

			ray r; l(r);

			forward_trace(r, camera.bounces);
		}

		if(use_global){
			// consider seeding random coords
			/*
			int64_t coord = x % (camera.w*camera.h),
					x_coord = coord % camera.w,
					y_coord = coord / camera.w;
			*/

			int x_coord = std::floor(rng::base() * camera.w),
				y_coord = std::floor(rng::base() * camera.h);

			ray r;

			r.c = global;
			r.p = camera.p;
			r.d = camera.d.project({
				(x_coord-camera.w/2 + rng::base()) / camera.c,
				(camera.h/2-y_coord + rng::base()) / camera.c,
				1,
			}).norm();

			if(backward_trace(r, camera.bounces))
				camera.add(x_coord, y_coord, r.c);

		}

		if(camera.report && x%camera.report == 0)
			camera.write("image.bmp", "image.raw", camera.adjust ? (double) x / camera.iter : 1.0);
	}

	camera.write("image.bmp", "image.raw", 1.0);
}
