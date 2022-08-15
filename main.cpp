#include <iostream>
#include "Lexan.h"

int main() {
	Lexan lexan;
	if(lexan.parse("file.txt")) {
		lexan.printTokens();
	}

}