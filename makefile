CXX = g++

CXXFLAGS = -Wall -Wextra -Wpedantic -Werror -O2 -std=c++17

all: main

main: main.o hash.o
	g++ $(CXXFLAGS) -o main main.o hash.o

main.o: main.cc hash.h
	g++ $(CXXFLAGS) -c main.cc

hash.o: hash.h hash.cc
	g++ $(CXXFLAGS) -c hash.cc

clean:
	$(RM) main.o hash.o

test_debug:
	echo $(FLAGS)

	g++ -Wall -Wextra -O0 -std=c++17 -c hash.cc -o hash.o -fsanitize=address -fno-omit-frame-pointer
	gcc -Wall -Wextra -O0 -std=c17 -c hash_test1.c -o hash_test1.o -fsanitize=address -fno-omit-frame-pointer
	g++ -Wall -Wextra -O0 -std=c++17 -c hash_test2.cc -o hash_test2.o -fsanitize=address -fno-omit-frame-pointer

	g++ hash_test1.o hash.o -o hash_test1 -fsanitize=address -fno-omit-frame-pointer
	g++ hash.o hash_test1.o -o hash_test12 -fsanitize=address -fno-omit-frame-pointer

	g++ hash_test2.o hash.o -o hash_test21 -fsanitize=address -fno-omit-frame-pointer
	g++ hash.o hash_test2.o -o hash_test22 -fsanitize=address -fno-omit-frame-pointer
	
	./hash_test1
	./hash_test12
	./hash_test21
	./hash_test22


test:
	g++ -Wall -Wextra -O2 -std=c++17 -c hash.cc -o hash.o 
	g++ -Wall -Wextra -O0 -std=c++17 -c hash.cc -o ash.o 
	g++ -DNDEBUG -Wall -Wextra -O2 -std=c++17 -c hash.cc -o hash.o
	gcc -Wall -Wextra -O2 -std=c17 -c hash_test1.c -o hash_test1.o
	g++ -Wall -Wextra -O2 -std=c++17 -c hash_test2.cc -o hash_test2.o

	g++ hash_test1.o hash.o -o hash_test1
	g++ hash.o hash_test1.o -o hash_test12 

	g++ hash_test2.o hash.o -o hash_test21 
	g++ hash.o hash_test2.o -o hash_test22
	
	nm hash.o
	
	./hash_test1
	./hash_test12
	./hash_test21
	./hash_test22
