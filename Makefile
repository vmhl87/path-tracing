threads = 1
length = 1
frame = 0
fps = 15

all: test

debug:
	g++ main.cpp -o ./run -g -lm -Wall -Wextra
	gdb run

run: *.cpp
	g++ main.cpp -o ./run -O2 -lm -march=native -Wall -Wextra

test: run
	./run $(threads) $(frame) $(fps)
	convert image.bmp image.png

perf:
	g++ main.cpp -o ./run -pg -g -lm -Wall -Wextra -O2 -march=native
	./run
	gprof run

video: run
	bash .video.sh $(threads) $(length) $(fps)
