all: test

debug:
	g++ main.cpp -o ./run -g -lm -Wall -Wextra
	gdb run

run: *.cpp
	g++ main.cpp -o ./run -O2 -lm -march=native -Wall -Wextra

test: run
	bash -c "time ./run"
	convert image.bmp image.png

perf:
	g++ main.cpp -o ./run -pg -g -lm -Wall -Wextra -O2 -march=native
	bash -c "time ./run"
	gprof run
