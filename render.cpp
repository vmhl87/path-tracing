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
end;

handle(material)
	case(c) c.read_vec();
	case(shine) c.read_double();
end;

handle(sphere)
	case(p) c.read_vec();
	case(r) c.read_double();
	case(mat) handle_material(c);
end;

#undef handle
#undef case
#undef end

#define case(x) }else if(c.s == #x){

void handle(node &c){
	if(c.s == ""){
		case(camera) camera = handle_cam(c);
		case(sphere) spheres.push_back(handle_sphere(c));
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
	
	for(int64_t x=0; x<camera.iter; ++x){
		ray r; r.c = {1, 1, 1};

		/*
		// skylight
		r.p = rng::uniform_norm().norm()*0.15 + (vec){0, 5, 0};
		r.d = (rng::uniform_norm() + (vec){0, -1, 0}).norm();
		*/

		/*
		// laser
		r.p = rng::gaussian()*0.02 + (vec){0, 0.5, 0};
		r.d = ((vec){-2, 0, 0} - (vec){0, 0.5, 0}).norm();
		*/

		/*
		// spotlight from camera
		r.p = camera.p;
		r.d = camera.d.project(rng::uniform_norm() + (vec){0, 0, 1}).norm();
		*/

		// combination
		if(rng::uniform_norm().x > 0 && rng::uniform_norm().x > 0){
			r.p = camera.p + (vec){0, 2, 0} + rng::gaussian() * 0.015;
			vec center = camera.p + camera.d.f * 3.0;
			r.d = (center-r.p).norm();
		}else{
			r.p = rng::uniform_norm().norm()*0.15 + (vec){2.1, 5, 3.2};
			r.d = (rng::uniform_norm() + (vec){0, -1, 0}).norm();
		}

		trace(r, camera.bounces);

		if(camera.report && x%camera.report == 0)
			camera.write("image.bmp", "image.raw", (double) x / camera.iter);
	}

	camera.write("image.bmp", "image.raw", 1.0);
}
