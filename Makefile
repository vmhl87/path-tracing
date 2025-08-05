all: render format convert

render: *.cpp
	g++ render.cpp -o ./run -lm -march=native -Wall

format: scene.conf
	cat format.sh | rg -v "^\s*$$"
	bash format.sh

convert: image.bmp
	convert image.bmp image.png
