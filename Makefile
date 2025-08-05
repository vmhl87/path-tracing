all: run .proc image.png

debug:
	g++ render.cpp -o ./run -g -lm -Wall
	gdb run

test:
	g++ render.cpp -o run -lm
	./run < scene.conf

run: *.cpp
	g++ render.cpp -o ./run -lm -march=native -Wall

.proc: scene.conf run
	touch .proc
	cat format.sh | rg -v "^\s*$$"
	bash format.sh

image.png: image.bmp
	convert image.bmp image.png

loop: run
	bash loop.sh
