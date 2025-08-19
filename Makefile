all: test

debug:
	g++ main.cpp -o ./run -g -lm -Wall -Wextra
	gdb run

run: *.cpp
	g++ main.cpp -o ./run -O2 -lm -march=native -Wall -Wextra

threads = 1
test: run
	./run $(threads)
	convert image.bmp image.png

perf:
	g++ main.cpp -o ./run -pg -g -lm -Wall -Wextra -O2 -march=native
	./run
	gprof run
