CC=g++
CFLAGS=-I.

default: src/main.cpp
	mkdir -p build
	$(CC) -std=c++11 -g -o build/shadertoy src/main.cpp -lSDL2 -lGL 


