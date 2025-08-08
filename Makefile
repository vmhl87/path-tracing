all: test

threads = 4
multi: run postprocess
	bash .multi.sh $(threads)

debug:
	g++ render.cpp -o ./run -g -lm -Wall
	gdb run

run: *.cpp
	g++ render.cpp -o ./run -lm -O2 -march=native -Wall

postprocess: postprocess.cpp
	g++ postprocess.cpp -o ./postprocess -O2 -march=native -Wall

test: run
	bash -c "time ./run < scene.conf"
	convert image.bmp image.png

final:
	convert image.bmp image.png
	cp image.png final-render.png

loop: run
	bash loop.sh

id = -tmp
demo:
	convert image.bmp image.png
	mkdir -p "demo$(id)"
	cp scene.conf image.png "demo$(id)"
	sed -e '/CODE/r scene.conf' -e '/CODE/d' template.md \
		> "demo$(id)/README.md"
