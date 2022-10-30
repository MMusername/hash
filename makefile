CXX = g++-10

CXXFLAGS = -Wall -Wextra -O2 -std=c++17

all: main

main: main.o hash.o
	g++-10 $(CXXFLAGS) -o main main.o hash.o

main.o: main.cc hash.h
	g++-10 $(CXXFLAGS) -c main.cc

hash.o: hash.h hash.cc
	g++-10 $(CXXFLAGS) -c hash.cc

clean:
	$(RM) main.o hash.o
