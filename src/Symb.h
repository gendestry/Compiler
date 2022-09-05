#pragma once
#include <string>
#include "Ast.h"


class AstDecl;

struct Symb {
	std::string name;
	bool isType = false;
	int depth = 0;
	AstDecl* decl;
};

enum Phase {
	HEAD,
	BODY
};
