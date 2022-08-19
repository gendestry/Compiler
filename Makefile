.phony: clean

all: bin/main

bin/main: src/main.cpp src/Lexan.cpp src/Font.cpp src/Logger.cpp
	g++ $^ -O3 --std=c++17 -o $@

run: bin/main
	./$^

clean:
	rm bin/*