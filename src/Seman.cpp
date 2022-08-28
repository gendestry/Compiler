#include "Seman.h"


bool Seman::resolveNames() {
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

// bool Seman::typeResolver() {
// 	return true;
// }

// bool Seman::lvalueResolver() {
// 	return true;
// }
