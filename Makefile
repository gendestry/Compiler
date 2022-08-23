.phony: clean

all: bin/main
debug: bin/main-debug

bin/main: src/main.cpp src/Lexan.cpp src/Font.cpp src/Logger.cpp src/Synan.cpp src/Ast.cpp src/Token.cpp
	g++ $^ -O3 --std=c++17 -o $@

bin/main-debug: src/main.cpp src/Lexan.cpp src/Font.cpp src/Logger.cpp src/Synan.cpp src/Ast.cpp src/Token.cpp
	g++ $^ -g --std=c++17 -o $@

run: bin/main
	./$^

clean:
	rm bin/*