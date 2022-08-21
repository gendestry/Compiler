#include <iostream>
#include "Synan.h"
#include "Lexan.h"
#include "Logger.h"

int main() {
	std::string filename = "file.txt";

	Lexan lexan;
	if(!lexan.parse(filename))
		return -1;

	lexan.printTokens();

	Synan synan(lexan);
	if(!synan.parse())
		return -1;
}