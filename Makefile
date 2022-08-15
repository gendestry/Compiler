
all: main

main: main.cpp Lexan.cpp
	g++ $^ -O3 --std=c++17 -o $@

run: main
	./$^