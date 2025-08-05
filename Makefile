all: run .proc convert

run: *.cpp
	g++ render.cpp -o ./run -lm -march=native -Wall

.proc: scene.conf run
	touch .proc
	cat format.sh | rg -v "^\s*$$"
	bash format.sh

convert: image.bmp
	convert image.bmp image.png
