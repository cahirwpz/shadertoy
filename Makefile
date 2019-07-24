CXX      = g++
CXXFLAGS = -std=c++11 -g
CPPFLAGS = $(shell pkg-config --cflags SDL2)
CC       = g++
LDFLAGS  = -g
LDLIBS	 = $(shell pkg-config --libs SDL2)

UNAME	 = $(shell uname)

ifeq ($(UNAME), Darwin)
  LDLIBS += -framework OpenGL
else
  LDLIBS += -lGL
endif

shadertoy: shadertoy.o
shadertoy.o: shadertoy.cpp

clean:
	rm -f shadertoy shadertoy.o *~
