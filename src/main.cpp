#include <iostream>
#include "Lexan.h"
#include "Synan.h"
#include "Seman.h"
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

	synan.printDecls();

	Seman seman(synan);
	if(!seman.resolveNames())
		return -1;
	
	if(!seman.resolveTypes())
		return -1;

	return 0;
}
