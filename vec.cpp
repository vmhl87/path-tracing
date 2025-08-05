#pragma once

struct vec3{
	double x, y, z;

	vec3 operator+(const vec3 o){
		return {x+o.x, y+o.y, z+o.z};
	}

	vec3 operator-(const vec3 o){
		return {x-o.x, y-o.y, z-o.z};
	}

	vec3 operator*(double v){
		return {x*v, y*v, z*v};
	}

	vec3 operator/(double v){
		return {x/v, y/v, z/v};
	}

	double mag(){
		return std::sqrt(x*x + y*y + z*z);
	}

	vec3 norm(){
		double m = mag();
		return {x/m, y/m, z/m};
	}

	double dot(const vec3 o){
		return x*o.x + y*o.y + z*o.z;
	}

	vec3 cross(const vec3 o){
		return {
			y*o.z - z*o.y,
			z*o.x - x*o.z,
			x*o.y - y*o.x,
		};
	}
};
