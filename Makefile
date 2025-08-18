all: test

debug:
	g++ main.cpp -o ./run -g -lm -Wall -std=c++20
	gdb run

run: *.cpp
	g++ main.cpp -o ./run -lm -O2 -march=native -Wall -std=c++20

test: run
	bash -c "time ./run"
	convert image.bmp image.png
