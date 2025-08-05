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

#define split for(node &c : n.child) if(c.s == "")
#define case(x) }else if(c.s == "##x##"){

void handle_dir(node &n){
	dir *d = new dir;
	
	split{
		case(f)
			d->f = c.read_vec(1);

		case(f)
			d->u = c.read_vec(1);
	}

	n.dat = (void*) d;
}

void handle_camera(node &n){
	split{
		case(p)
			camera.p = c.read_vec(1);

		case(d)
			handle_dir(c);
			camera.d = *((dir*) c.dat);

		case(w)
			camera.w = c.read_int(1);

		case(h)
			camera.h = c.read_int(1);

		case(c)
			camera.c = c.read_double(1);
	}
}

void handle_sphere(node &n){
	split{
	}
}

#undef split
#define split if(c.s == "")

void handle(node &c){
	split{
		case(camera)
			handle_camera(c);

		case(sphere)
			handle_sphere(c);
	}
}

#undef split
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
