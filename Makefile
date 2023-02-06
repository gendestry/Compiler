.phony: clean

all: bin/compiler
debug: bin/compiler-debug

FILES = $(wildcard src/*.cpp)

bin/compiler: $(FILES)
	g++ $^ -O3 --std=c++17 -o $@

bin/compiler-debug: $(FILES)
	g++ $^ -g --std=c++17 -o $@

run: bin/compiler
	./$^

clean:
	rm bin/*
