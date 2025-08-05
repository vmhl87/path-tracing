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
	std::vector<node> child;
	bool root;
	void *dat;

	node(std::string S, int r){
		root = r == 0;
		s = S;
	}

#define read(type) type read_##type(int leading){ \
	std::istringstream v(s); \
	std::string S; \
	for(int i=0; i<leading; ++i) v >> S; \
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
	for(node &c : n.child) if(c.s == ""){

#define end } return res; }

#define case(x) }else if(c.s == "##x##"){ res. x =

handle(dir)
	case(f) c.read_vec(1);
	case(f) c.read_vec(1);
end;

handle(cam)
	case(p) c.read_vec(1);
	case(d) handle_dir(c);
	case(w) c.read_int(1);
	case(h) c.read_int(1);
	case(c) c.read_double(1);
end;

handle(material)
	case(c) c.read_vec(1);
	case(shine) c.read_double(1);
end;

handle(sphere)
	case(p) c.read_vec(1);
	case(r) c.read_double(1);
	case(mat) handle_material(c);
end;

#undef handle
#undef case
#undef end

#define case(x) }else if(c.s == "##x##"){

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

	std::vector<node> nodes;
	std::vector<int> ctx;

	while(true){
		std::getline(std::cin, s);
		if(s == "done") break;

		if(only_whitespace(s)) continue;
		int lvl = count_tabs(s);

		if(lvl > level) ctx.push_back(nodes.size()-1);
		else if(lvl < level) ctx.pop_back();
		level = lvl;

		node x(s, ctx.size());
		if(ctx.size()) nodes[ctx.back()].child.push_back(x);
		nodes.push_back(x);
	}

	for(node &n : nodes) if(n.root) handle(n);
}
