#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Token.h"


class Lexan {
	std::vector<Token> tokens;
public:
	Lexan();

	bool parse(const std::string& file);
	bool parseLine(std::string& line, int i);

	void printTokens();

	const std::vector<Token>& getTokens() const { return tokens; }
};
