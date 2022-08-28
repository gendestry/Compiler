#include "NameResolver.h"
#include "Ast.h"
#include "Logger.h"

void NameResolver::clearSymbolDepth() {
	for(auto it = symbolTable.rbegin(); it != symbolTable.rend(); it++) {
		if(it->depth > depth) {
			symbolTable.erase((it + 1).base());
		}
	}
}

bool NameResolver::isNameValid(const std::string& name, bool type) {
	for (auto& symb : symbolTable) {
		if (symb.name == name && symb.depth == depth && symb.isType == type) {
			return false;
		}
	}
	return true;
}

AstDecl* NameResolver::findDecl(const std::string& name, bool type) {
	Logger::getInstance().debug("Looking for %s[%d]", name.c_str(), type);
	for(auto it = symbolTable.rbegin(); it != symbolTable.rend(); it++) {
		Logger::getInstance().debug(" - at %s[%d, %d]", it->name.c_str(), it->depth, it->isType);
		if(it->name == name && it->isType == type) {
			return it->decl;
		}
	}

	return nullptr;
}



bool NameResolver::visit(AstVarDecl* varDecl, Phase phase) {
	if(phase == Phase::HEAD) {
		if(!isNameValid(varDecl->name)) {
			Logger::getInstance().error("Name error: variable %s[%d] redeclared!", varDecl->name.c_str(), depth);
			return false;
		}
		symbolTable.push_back({ varDecl->name, false, depth , varDecl});
	}
	else {
		if(!varDecl->type->accept(this, phase)) {
			return false;
		}

		if(varDecl->expr && !varDecl->expr->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstParDecl* parDecl, Phase phase) {
	for(auto& decl : parDecl->params) {
		if(!decl.accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstFunDecl* funDecl, Phase phase) {
	if(phase == Phase::HEAD) {
		if(!isNameValid(funDecl->name)) {
			Logger::getInstance().error("Name error: Function %s[%d] redeclared!", funDecl->name.c_str(), depth);
			return false;
		}
		symbolTable.push_back({ funDecl->name, false, depth , funDecl});
	}
	else {
		if(!funDecl->type->accept(this, phase)) {
			return false;
		}
		
		depth++;

		if(funDecl->params && !funDecl->params->accept(this, Phase::HEAD)) {
			return false;
		}

		if(funDecl->params && !funDecl->params->accept(this, Phase::BODY)) {
			return false;
		}

		depth--;

		if(funDecl->body && !funDecl->body->accept(this, phase)) {
			return false;
		}

		// no need to cleanup the symbol table because comp stmt clears it
	}

	return true;
}

bool NameResolver::visit(AstTypeDecl* typeDecl, Phase phase) {
	if(phase == Phase::HEAD) {
		if(!isNameValid(typeDecl->name, true)) {
			Logger::getInstance().error("Name error: Type %s[%d] redeclared!", typeDecl->name.c_str(), depth);
			return false;
		}
		symbolTable.push_back({ typeDecl->name, true, depth , typeDecl});
	}
	else {
		if(!typeDecl->type->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstStructDecl* structDecl, Phase phase) {
	if(phase == Phase::HEAD) {
		if(!isNameValid(structDecl->name)) {
			Logger::getInstance().error("Name error: Struct %s[%d] redeclared!", structDecl->name.c_str(), depth);
			return false;
		}
		symbolTable.push_back({ structDecl->name, false, depth , structDecl});
	}
	else {
		depth++;

		for(auto& decl : structDecl->fields) {
			if(!decl.accept(this, Phase::HEAD))
				return false;
		}

		for(auto& decl : structDecl->fields) {
			if(!decl.accept(this, Phase::BODY))
				return false;
		}

		depth--;

		clearSymbolDepth();
	}

	return true;
}



bool NameResolver::visit(AstAtomType* atomType, Phase phase) {
	return true;
}

bool NameResolver::visit(AstNamedType* namedType, Phase phase) {
	if(phase == Phase::BODY) {
		AstDecl* decl = findDecl(namedType->name, true);

		if(decl == nullptr) {
			Logger::getInstance().error("Name error: Type %s not found!", namedType->name.c_str());
			return false;
		}

		namedType->declaration = decl;
		Logger::getInstance().log("Found type %s", decl->toString().c_str());
	}

	return true;
}

bool NameResolver::visit(AstPtrType* ptrType, Phase phase) {
	if(phase == Phase::BODY) {
		if(!ptrType->ptrType->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstArrayType* arrayType, Phase phase) {
	if(phase == Phase::BODY) {
		if(!arrayType->arrayType->accept(this, phase)) {
			return false;
		}

		if(!arrayType->expr->accept(this, phase)) {
			return false;
		}
	}

	return true;
}



bool NameResolver::visit(AstConstExpr* constExpr, Phase phase) {
	return true;
}

bool NameResolver::visit(AstNamedExpr* namedExpr, Phase phase) {
	if(phase == Phase::BODY) {
		AstDecl* decl = findDecl(namedExpr->name, false);

		if(decl == nullptr) {
			Logger::getInstance().error("Name error: Variable %s not found!", namedExpr->name.c_str());
			return false;
		}

		namedExpr->declaration = decl;
		Logger::getInstance().log("Found variable %s", decl->toString().c_str());
	}

	return true;
}

bool NameResolver::visit(AstCallExpr* callExpr, Phase phase) {
	if(phase == Phase::BODY) {
		AstDecl* decl = findDecl(callExpr->name, false);

		if(decl == nullptr) {
			Logger::getInstance().error("Name error: Function %s not found!", callExpr->name.c_str());
			return false;
		}

		callExpr->declaration = decl;
		Logger::getInstance().log("Found function %s", decl->toString().c_str());

		for(auto& expr : callExpr->args) {
			if(!expr->accept(this, phase)) {
				return false;
			}
		}
	}

	return true;
}

bool NameResolver::visit(AstCastExpr* castExpr, Phase phase) {
	if(phase == Phase::BODY) {
		if(!castExpr->type->accept(this, phase)) {
			return false;
		}

		if(!castExpr->expr->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstPrefixExpr* prefixExpr, Phase phase) {
	if(phase == Phase::BODY) {
		if(!prefixExpr->expr->accept(this, phase)) {
			return false;
		}
	}
	
	return true;
}

bool NameResolver::visit(AstPostfixExpr* postfixExpr, Phase phase) {
	if(phase == Phase::BODY) {
		if(!postfixExpr->expr->accept(this, phase)) {
			return false;
		}
	}
	
	return true;
}

bool NameResolver::visit(AstBinaryExpr* binaryExpr, Phase phase) {
	if(phase == Phase::BODY) {
		if(!binaryExpr->left->accept(this, phase)) {
			return false;
		}
		if(!binaryExpr->right->accept(this, phase)) {
			return false;
		}
	}

	return true;
}


bool NameResolver::visit(AstExprStmt* exprStmt, Phase phase) {
	if(phase == Phase::BODY) {
		if(!exprStmt->expr->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstAssignStmt* assignStmt, Phase phase) {
	if(phase == Phase::BODY) {
		if(!assignStmt->left->accept(this, phase)) {
			return false;
		}
		if(!assignStmt->right->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstCompStmt* compStmt, Phase phase) {
	depth++;

	for(auto& stmt : compStmt->stmts) {
		if(!stmt->accept(this, Phase::BODY)) {
			return false;
		}

		if(!stmt->accept(this, Phase::HEAD)) {
			return false;
		}
	}

	depth--;

	clearSymbolDepth();

	return true;
}

bool NameResolver::visit(AstIfStmt* ifStmt, Phase phase) {
	if(phase == Phase::BODY) {
		if(!ifStmt->cond->accept(this, phase)) {
			return false;
		}

		if(!ifStmt->stmt->accept(this, phase)) {
			return false;
		}

		if(ifStmt->elseStmt && !ifStmt->elseStmt->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstWhileStmt* whileStmt, Phase phase) {
	if(phase == Phase::BODY) {
		if(!whileStmt->cond->accept(this, phase)) {
			return false;
		}

		if(!whileStmt->stmt->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstReturnStmt* returnStmt, Phase phase) {
	if(phase == Phase::BODY) {
		if(returnStmt->expr && !returnStmt->expr->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool NameResolver::visit(AstVarStmt* varStmt, Phase phase) {
	return varStmt->decl.accept(this, phase);
}
