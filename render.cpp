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
	case(chunk) c.read_int64_t();
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
	case(granular) c.read_int();
end;

handle(material)
	case(c) c.read_vec();  // deprecated
	case(shine) c.read_double();
	case(gloss) c.read_double();
end;

handle(sphere)
	case(p) c.read_vec();
	case(d) c.read_vec();
	case(r) c.read_double();
	case(mat) handle_material(c); res.mat.c = c.read_vec();
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

			for(int64_t i=0; i<camera.chunk; ++i){
				ray r; l(r); r.t = rng::base();

				if(l.visible){
					ray R;
					R.p = r.p;
					R.d = (camera.p-r.p).norm();
					R.c = r.c;
					add(R);
				}

				forward_trace(r, camera.bounces);
			}
		}

		if(use_global) for(int64_t i=0; i<camera.chunk; ++i){
			// load balanced
			int x_coord, y_coord; camera.pick(x_coord, y_coord);

			ray r;

			r.c = global, r.p = camera.p; r.t = rng::base();
			r.d = camera.d.project({
				x_coord-camera.w/2 + rng::base(),
				1+camera.h/2-y_coord + rng::base(),
				camera.c,
			}).norm();

			if(backward_trace(r, camera.bounces))
				camera.add(x_coord, y_coord, r.c);
				//camera.add(camera.w/2+std::floor(x_coord), camera.h/2+std::floor(y_coord), r.c);
		}

		if(camera.report && x%camera.report == 0)
			camera.write("image.bmp", "image.raw", camera.adjust ? (double) x / camera.iter : 1.0);
	}

	camera.write("image.bmp", "image.raw", 1.0);
}
