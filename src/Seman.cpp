#include "Seman.h"


bool Seman::resolveNames() {
	Logger::getInstance().log("#i#grnPhase 3.1: Name Resolving#r\n");

	for(auto& decl : decls) {
		if(!decl->accept(&nameResolver, Phase::HEAD)) {
			return false;
		}
	}

	for(auto& decl : decls) {
		if(!decl->accept(&nameResolver, Phase::BODY)) {
			return false;
		}
	}

	return true;
}

bool Seman::resolveTypes() {
	Logger::getInstance().log("#i#grnPhase 3.2: Type Resolving#r\n");

	for(auto& decl : decls) {
		if(!decl->accept(&typeResolver, Phase::HEAD)) {
			return false;
		}
	}

	return true;
}

// bool Seman::lvalueResolver() {
// 	return true;
// }
