CC=g++
CFLAGS=-I.
UNAME=$(shell uname)


default: src/main.cpp
	mkdir -p build
ifeq ($(UNAME), Darwin)
	$(CC) -std=c++11 -g -o build/shadertoy src/main.cpp -lSDL2 -framework OpenGL
else
	$(CC) -std=c++11 -g -o build/shadertoy src/main.cpp -lSDL2 -lGL
endif
