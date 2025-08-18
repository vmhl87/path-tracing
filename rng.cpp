#pragma once

#include <climits>
#include <random>
#include <cmath>

namespace rng{
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_real_distribution<double> uniform_gen(0.0, 1.0);
	std::normal_distribution<double> gaussian_gen(0.0, 1.0);

	void init(){
		srand(time(NULL));
	}

	inline double base(){
		//return ((double)xorshf96()/(double)UINT_MAX);
		//return ((double)rand()/(double)RAND_MAX);
		return uniform_gen(gen);
	}

	inline double norm(){
		return gaussian_gen(gen);
	}

	vec uniform_norm(){
		double u = base() * 2.0 - 1.0;
		double phi = base() * 2.0 * M_PI;
		double sqrt_1_minus_u2 = sqrt(1.0 - u * u);

		return {
			sqrt_1_minus_u2 * cos(phi),
			sqrt_1_minus_u2 * sin(phi),
			u
		};
	}

	vec uniform(){
		return uniform_norm() * pow(base(), 1.0/3.0);
	}

	vec gaussian(){
		return uniform_norm() * norm();
	}
}
