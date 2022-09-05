#pragma once
#include <string>
#include <iostream>

class Location {
public:
	int line = 0;
	int start = 0;
	int end = 0;

public:
	Location() {}
	Location(int line, int start, int end) : line(line), start(start), end(end) { }

	std::string toString() const { return " on line " + std::to_string(line); }// + ":" + std::to_string(start) + "-" + std::to_string(end) + "]"; }
};
