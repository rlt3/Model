CFLAGS=-Wall -g -ggdb -std=c++11
LDFLAGS=-lSDL2 -lGL -lGLU -lm

all:
	$(CXX) $(CFLAGS) -o model main.cpp draw.cpp $(LDFLAGS) 
