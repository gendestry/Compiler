#include <iostream>
#include "Lexan.h"
#include "Logger.h"

int main() {
	Lexan lexan;

	if(!lexan.parse("file.txt")) {
		Logger::getInstance().error("Lexan: Could not open parse file!");
		return -1;
	}

}