#pragma once
#include <vector>
#include "Logger.h"
#include "Synan.h"
#include "Symb.h"

class Seman {
public:
	Seman(Synan& synan) : decls(synan.getDecls()) {
		Logger::getInstance().log("#i#grnPhase 3: Semantic analysis#r\n");
	}

	bool nameResolver();
	bool typeResolver();
	bool lvalueResolver();

private:
	std::vector<AstDecl*>& decls;
	std::vector<Symb> declaredAt;
};