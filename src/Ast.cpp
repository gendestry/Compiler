#include "Ast.h"
#include "Logger.h"

static int nameDepth = 0;

bool check(std::string name, std::vector<Symb>& declaredAt) {
	for (auto& symb : declaredAt) {
		if (symb.name == name && symb.depth == nameDepth) {
			return false;
		}
	}
	return true;
}

bool AstVarDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {
	if(phase == Phase::HEAD) {
		if(check(name, declaredAt)) {
			declaredAt.push_back({name, nameDepth, this});
		}
		else {
			Logger::getInstance().error("Seman: Variable " + name + " already declared in this scope\n");
			return false;
		}
	}
	else {
		if(!type->visitName(phase, declaredAt))
			return false;
		
		if(expr && !expr->visitName(phase, declaredAt)) {
			return false;
		}
	}

	return true;
}

bool AstTypeDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {
	if(phase == Phase::HEAD) {
		if(check(name, declaredAt)) {
			declaredAt.push_back({name, nameDepth, this});
		}
		else {
			Logger::getInstance().error("Seman: Type " + name + " already declared in this scope\n");
			return false;
		}
	}
	else {
		if(!type->visitName(phase, declaredAt))
			return false;
	}

	return true;
}

bool AstParDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {
	if(phase == Phase::HEAD) {
		for(auto& decl : params) {
			if(!decl.visitName(phase, declaredAt)) {
				return false;
			}
		}
	}
	else {
		for(auto& decl : params) {
			if(!decl.visitName(phase, declaredAt)) {
				return false;
			}
		}
	}

	return true;
}

bool AstFunDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {
	if(phase == Phase::HEAD) {
		if(check(name, declaredAt)) {
			declaredAt.push_back({name, nameDepth, this});
		}
		else {
			Logger::getInstance().error("Seman: Function " + name + " already declared in this scope\n");
			return false;
		}
	}
	else {
		if(!type->visitName(phase, declaredAt))
			return false;
		
		nameDepth++;

		if(params && !params->visitName(phase, declaredAt))
			return false;
		
		if(body && !body->visitName(phase, declaredAt))
			return false;

		nameDepth--;
	}

	return true;
}

bool AstStructDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {
	if(phase == Phase::HEAD) {
		if(check(name, declaredAt)) {
			declaredAt.push_back({name, nameDepth, this});
		}
		else {
			Logger::getInstance().error("Seman: Struct " + name + " already declared in this scope\n");
			return false;
		}
	}
	else {
		// for(auto& decl : fields) {
		// 	decl.visitName(phase, declaredAt);
		// }
	}
}



bool AstAtomType::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstNamedType::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstPtrType::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstArrayType::visitName(Phase phase, std::vector<Symb>& declaredAt) {}



bool AstConstExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstNamedExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstCallExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstCastExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstPrefixExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstPostfixExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstBinaryExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}



bool AstExprStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstAssignStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstCompStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstIfStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstWhileStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstReturnStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

bool AstVarStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}



// bool AstVarDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstTypeDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstParDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstFunDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstStructDecl::visitName(Phase phase, std::vector<Symb>& declaredAt) {}



// bool AstAtomType::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstNamedType::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstPtrType::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstArrayType::visitName(Phase phase, std::vector<Symb>& declaredAt) {}



// bool AstConstExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstNamedExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstCallExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstCastExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstPrefixExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstPostfixExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstBinaryExpr::visitName(Phase phase, std::vector<Symb>& declaredAt) {}



// bool AstExprStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstAssignStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstCompStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstIfStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstWhileStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstReturnStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}

// bool AstVarStmt::visitName(Phase phase, std::vector<Symb>& declaredAt) {}



std::string AstDecl::toString() const {
	return "AstDecl";
}

std::string AstVarDecl::toString() const {
	return "VarDecl[" + name + " : " + type->toString() + (expr ? " = " + expr->toString() : "") + "]";
}

std::string AstTypeDecl::toString() const {
	return "TypeDecl[" + name + " : " + type->toString() + "]";
}

std::string AstParDecl::toString() const {
	std::string str = "ParDecl[";
	for (int i = 0; i < params.size(); i++) {
		str += params[i].toString();
		if (i < params.size() - 1)
			str += ", ";
	}
	str += "]";
	return str;
}

std::string AstFunDecl::toString() const {
	return "FunDecl[" + name + " : " + type->toString() + (params ? (", " + params->toString()) : "") + (body ? (", " + body->toString()) : "") + "]";
}

std::string AstStructDecl::toString() const {
	std::string str = "StructDecl[" + name + " : ";
	for (int i = 0; i < fields.size(); i++) {
		str += fields[i].toString();
		if (i < fields.size() - 1)
			str += ", ";
	}
	str += "]";
	return str;
}



std::string AstType::toString() const {
	return "AstType";
}

std::string AstAtomType::toString() const {
	std::string ret = "AtomType[";
	switch(type) {
		case INT:
			ret += "int]";
			break;
		case CHAR:
			ret += "char]";
			break;
		case BOOL:
			ret += "bool]";
			break;
		case VOID:
			ret += "void]";
			break;
		case FLOAT:
			ret += "float]";
			break;
		case NIL:
			ret += "nil]";
			break;
	}

	return ret;
}

std::string AstNamedType::toString() const {
	return "NamedType[" + name + "]";
}

std::string AstPtrType::toString() const {
	return "PtrType[" + type->toString() + "," + std::to_string(depth) + "]";
}

std::string AstArrayType::toString() const {
	return "ArrayType[" + type->toString() + "," + expr->toString() + "]";
}



std::string AstExpr::toString() const {
	return "AstExpr";
}

std::string AstConstExpr::toString() const {
	std::string ret = "AstConstExpr[";
	switch(type) {
		case Token::NUMBER:
			ret += std::to_string(ivalue) + "]";
			break;
		case Token::CHARACTER:
			ret += "'";
			ret += cvalue;
			ret += "']";
			break;
		case Token::TRUE:
			ret += "true]";
			break;
		case Token::FALSE:
			ret += "false]";
			break;
		case Token::STRING:
			ret += "\"" + str + "\"]";
			break;
		case Token::FNUMBER:
			ret += std::to_string(fvalue) + "]";
			break;
		// case Token::NIL:
		// 	ret += "nil]";
		// 	break;
	}
	return ret;
}

std::string AstNamedExpr::toString() const {
	return "AstNamedExpr[" + name + "]";
}

std::string AstCallExpr::toString() const {
	std::string str = "AstCallExpr[" + name + "(";
	for (int i = 0; i < args.size(); i++) {
		str += args[i]->toString();
		if (i < args.size() - 1)
			str += ", ";
	}
	str += ")]";
	return str;
}

std::string AstCastExpr::toString() const {
	return "AstCastExpr[" + type->toString() + ", " + expr->toString() + "]";
}

std::string AstPrefixExpr::toString() const {
	std::string ret = "AstPrefixExpr[";
	switch(op) {
		case PLUS:
			ret += "+";
			break;
		case MINUS:
			ret += "-";
			break;
		case PPLUS:
			ret += "++";
			break;
		case MMINUS:
			ret += "--";
			break;
		case NOT:
			ret += "!";
			break;
		case NEGATE:
			ret += "~";
			break;
		case DEREF:
			ret += "*";
			break;
		case ADDR:
			ret += "&";
			break;
		default:
			ret += std::to_string(op);
	}
	ret += ", " + expr->toString() + "]";
	return ret;
}

std::string AstPostfixExpr::toString() const {
	std::string ret = "AstPostfixExpr[";
	if(type == 0) {
		switch(op) {
			case PPLUS:
				ret += "++, ";
				break;
			case MMINUS:
				ret += "--, ";
				break;
		}
	}
	else if(type == 1) {
		switch(op) {
			case ACCESS:
				ret += ".access, ";
				break;
			case PTRACCESS:
				ret += "->access, ";
				break;
		}
		ret += name + ", ";
	}
	else if(type == 2)
		ret += "array, ";

	ret += expr->toString(); // + "]";
	if(type == 2)
		ret += ", " + index->toString();
	ret += "]";
	return ret;
}

std::string AstBinaryExpr::toString() const {
	return "AstBinExpr[" + Token::tokenNames[(int)op] + ", " + left->toString() + ", " + right->toString() + "]";
}



std::string AstStmt::toString() const {
	return "AstStmt";
}

std::string AstExprStmt::toString() const {
	return "AstExprStmt[" + expr->toString() + "]";
}

std::string AstAssignStmt::toString() const {
	return "AstAssignStmt[" + Token::tokenNames[(int)op] + ", " + left->toString() + ", " + right->toString() + "]";
}

std::string AstCompStmt::toString() const {
	std::string str = "AstCompStmt[";
	for (int i = 0; i < stmts.size(); i++) {
		str += stmts[i]->toString();
		if (i < stmts.size() - 1)
			str += ", ";
	}
	str += "]";
	return str;
}

std::string AstIfStmt::toString() const {
	return "AstIfStmt[" + cond->toString() + ", " + stmt->toString() + (elseStmt ? (", " + elseStmt->toString()) : "") + "]";
}

std::string AstWhileStmt::toString() const {
	return "AstWhileStmt[" + cond->toString() + ", " + stmt->toString() + "]";
}

std::string AstReturnStmt::toString() const {
	return "AstReturnStmt[" + (expr ? expr->toString() : "") + "]";
}

std::string AstVarStmt::toString() const {
	return "AstVarStmt[" + decl.toString() + "]";
}