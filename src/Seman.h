#pragma once
#include <vector>
#include "Logger.h"
#include "Synan.h"
#include "NameResolver.h"
#include "TypeResolver.h"

class Seman {
public:
	Seman(Synan& synan) : decls(synan.getDecls()) {
		Logger::getInstance().log("#i#grnPhase 3: Semantic analysis#r\n");
	}

	bool resolveNames();
	bool resolveTypes();

private:
	std::vector<AstDecl*>& decls;
	
	NameResolver nameResolver;
	TypeResolver typeResolver;
};
