#include "Seman.h"



bool Seman::nameResolver() {
	for(auto& decl : decls) {
		decl->visitName(Phase::HEAD, declaredAt);
	}
	for(auto& decl : decls) {
		decl->visitName(Phase::BODY, declaredAt);
	}
	return true;
}

bool Seman::typeResolver() {
	return true;
}

bool Seman::lvalueResolver() {
	return true;
}