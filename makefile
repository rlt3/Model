CFLAGS=-Wall -g -ggdb
LDFLAGS=-lSDL2 -lGL -lGLU -lm

all:
	$(CXX) $(CFLAGS) -o model main.cpp draw.cpp $(LDFLAGS) 
