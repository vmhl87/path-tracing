#include <random>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "trace.cpp"

bool only_whitespace(std::string s){
	for(char c : s)
		if(c != ' ' && c != '\t')
			return false;

	return true;
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

	read(vec) v >> res.x >> res.y >> res.z end

#undef read
#undef end

};

#define handle(type) type handle_##type(node &n){ \
	type res; \
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
	case(iter) c.read_int();
	case(noise) c.read_double();
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
	int level = 0;

	std::vector<node*> nodes;
	std::vector<int> ctx;

	while(true){
		std::getline(std::cin, s);
		if(s == "done") break;

		if(only_whitespace(s)) continue;
		int lvl = count_tabs(s);

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
	
	for(int x=0; x<camera.iter; ++x){
		for(int i=0; i<camera.w; ++i)
			for(int j=0; j<camera.h; ++j){
				ray r;

				r.p = camera.p;
				r.d = camera.d.project({
					(i-camera.w/2 + d(gen)*camera.noise) / camera.c,
					(camera.h/2-j + d(gen)*camera.noise) / camera.c,
					1,
				}).norm();

				r.c = {1, 1, 1};

				trace(r, 5);

				camera.add(i, j, r.c);
			}

		if(x%5 == 4) camera.write("image.bmp");
	}

	camera.write("image.bmp");
}
