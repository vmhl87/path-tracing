all: test

final: run .proc image.png

debug:
	g++ render.cpp -o ./run -g -lm -Wall
	gdb run

run: *.cpp
	g++ render.cpp -o ./run -lm -march=native -Wall

test: run
	./run < scene.conf

.proc: scene.conf run
	touch .proc
	cat format.sh | rg -v "^\s*$$"
	bash format.sh

image.png: image.bmp
	convert image.bmp image.png
	cp image.png final-render.png

loop: run
	bash loop.sh

id = -tmp
demo: test
	convert image.bmp image.png
	mkdir -p "demo$(id)"
	cp scene.conf image.png "demo$(id)"
	sed -e '/CODE/r scene.conf' -e '/CODE/d' template-demo.md \
		> "demo$(id)/README.md"
