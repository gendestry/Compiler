#include "Ast.h"
#include "Font.h"

static std::string typeColor = Font::byColorCode(144, 42, 181);
static std::string nameColor = Font::byColorCode(195, 180, 52);
static std::string valueColor = Font::byColorCode(39, 180, 99);
static std::string charColor = Font::byColorCode(200, 120, 40);
static std::string lvalueColor = Font::byColorCode(100, 170, 255);
static std::string stringColor = charColor;

static std::string colorize(std::string color, std::string var) {
	return color + var + Font::reset;
}


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



std::string AstType::getTypeName() const {
	std::string ret = "";

	switch(type) {
	case AstType::INT: ret = "int"; break;
	case AstType::CHAR: ret = "char"; break;
	case AstType::BOOL: ret = "bool"; break;
	case AstType::VOID: ret = "void"; break;
	case AstType::FLOAT: ret = "float"; break;
	case AstType::NAMED: ret = ((AstNamedType*)this)->name; break;
	case AstType::PTR: ret = ((AstPtrType*)this)->ptrType->getTypeName(); ret += "*"; break;
	case AstType::ARRAY: ret = ((AstArrayType*)this)->arrayType->getTypeName(); ret += "[]"; break;
	default: 
		ret = "UNKNOWN";
	}

	return ret;
}

std::string AstType::toString() const {
	return "AstType";
}

std::string AstAtomType::toString() const {
	std::string ret = "AtomType[";
	std::string type = getTypeName();
	
	return ret + type + "]";
}

std::string AstNamedType::toString() const {
	return "NamedType[" + name + "]";
}

std::string AstPtrType::toString() const {
	return "PtrType[" + ptrType->toString() + "]";
}

std::string AstArrayType::toString() const {
	return "ArrayType[" + arrayType->toString() + ", " + expr->toString() + "]";
}



std::string AstExpr::toString() const {
	return "AstExpr";
}

std::string AstConstExpr::toString() const {
	std::string lvalue = isLValue ? colorize(lvalueColor, "L") : "";
	std::string ret = "AstConstExpr" + lvalue;
	std::string temp = "";

	if(ofType) {
		ret += "{" + ofType->getTypeName() + "}";
	}
	ret += "[";
	switch(type) {
		case Token::NUMBER:
			ret += std::to_string(ivalue) + "]";
			break;
		case Token::CHARACTER:
			temp += "'";
			temp += cvalue;
			temp += "'";
			ret += temp + "]";
			break;
		case Token::TRUE:
			ret += "true]";
			break;
		case Token::FALSE:
			ret += "false]";
			break;
		case Token::STRING:
			temp += "\"" + str + "\"";
			ret += temp + "]";
			break;
		case Token::FNUMBER:
			ret += std::to_string(fvalue) + "]";
			break;
	}
	return ret;
}

std::string AstNamedExpr::toString() const {
	std::string ret = "AstNamedExpr";
	if(ofType) {
		ret += "{" + ofType->getTypeName() + "}";
	}
	ret += "[" + name + "]";
	return ret;
}

std::string AstCallExpr::toString() const {
	std::string str = "AstCallExpr";
	if(ofType) {
		str += "{" + ofType->getTypeName() + "}";
	}
	str += "[";
	str += name + "(";
	for (int i = 0; i < args.size(); i++) {
		str += args[i]->toString();
		if (i < args.size() - 1)
			str += ", ";
	}
	str += ")]";
	return str;
}

std::string AstCastExpr::toString() const {
	std::string ret = "AstCastExpr";
	if(ofType) {
		ret += "{" + ofType->getTypeName() + "}";
	}
	ret += "[";
	ret += type->toString() + ", " + expr->toString() + "]";
	return ret;
}

std::string AstPrefixExpr::toString() const {
	std::string ret = "AstPrefixExpr";
	if(ofType) {
		ret += "{" + ofType->getTypeName() + "}";
	}
	ret += "[";

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
	std::string ret = "AstPostfixExpr";
	if(ofType) {
		ret += "{" + ofType->getTypeName() + "}";
	}
	ret += "[";

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
	std::string ret = "AstBinExpr";
	if(ofType) {
		ret += "{" + ofType->getTypeName() + "}";
	}
	ret += "[";
	ret += Token::tokenNames[(int)op] + ", " + left->toString() + ", " + right->toString() + "]";
	return ret;
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

std::string AstFunStmt::toString() const {
	return "AstFunStmt[" + decl->toString() + "]";
}

// prettyToString()


std::string AstDecl::prettyToString() const {
	return "AstDecl";
}

std::string AstVarDecl::prettyToString() const {
	return "VarDecl[" + colorize(nameColor, name) + " : " + type->prettyToString() + (expr ? " = " + expr->prettyToString() : "") + "]";
}

std::string AstTypeDecl::prettyToString() const {
	return "TypeDecl[" + colorize(typeColor, name) + " : " + type->prettyToString() + "]";
}

std::string AstParDecl::prettyToString() const {
	std::string str = "ParDecl[";
	for (int i = 0; i < params.size(); i++) {
		str += params[i].prettyToString();
		if (i < params.size() - 1)
			str += ", ";
	}
	str += "]";
	return str;
}

std::string AstFunDecl::prettyToString() const {
	return "FunDecl[" + colorize(nameColor, name) + " : " + type->prettyToString() + (params ? (", " + params->prettyToString()) : "") + (body ? (", " + body->prettyToString()) : "") + "]";
}

std::string AstStructDecl::prettyToString() const {
	std::string str = "StructDecl[" + colorize(nameColor, name) + " : ";
	for (int i = 0; i < fields.size(); i++) {
		str += fields[i].prettyToString();
		if (i < fields.size() - 1)
			str += ", ";
	}
	str += "]";
	return str;
}



std::string AstType::prettyGetTypeName() const {
	std::string ret = "";

	switch(type) {
	case AstType::INT: ret = "int"; break;
	case AstType::CHAR: ret = "char"; break;
	case AstType::BOOL: ret = "bool"; break;
	case AstType::VOID: ret = "void"; break;
	case AstType::FLOAT: ret = "float"; break;
	case AstType::NAMED: ret = ((AstNamedType*)this)->name; break;
	case AstType::PTR: ret = ((AstPtrType*)this)->ptrType->prettyGetTypeName(); ret+= "*"; break;
	case AstType::ARRAY: ret = ((AstArrayType*)this)->arrayType->prettyGetTypeName(); ret +="[]"; break;
	default: 
		ret = "UNKNOWN";
	}

	return colorize(typeColor, ret);
}

std::string AstType::prettyToString() const {
	return "AstType";
}

std::string AstAtomType::prettyToString() const {
	std::string ret = "AtomType[";
	std::string type = prettyGetTypeName();
	
	return ret + colorize(typeColor, type) + "]";
}

std::string AstNamedType::prettyToString() const {
	return "NamedType[" + colorize(typeColor, name) + "]";
}

std::string AstPtrType::prettyToString() const {
	return "PtrType[" + ptrType->prettyToString() + "]";
}

std::string AstArrayType::prettyToString() const {
	return "ArrayType[" + arrayType->prettyToString() + ", " + expr->prettyToString() + "]";
}



std::string AstExpr::prettyToString() const {
	return "AstExpr";
}

std::string AstConstExpr::prettyToString() const {
	std::string ret = isLValue ? colorize(lvalueColor, "AstConstExpr") : "AstConstExpr";
	std::string temp = "";

	if(ofType) {
		ret += "{" + ofType->prettyGetTypeName() + "}";
	}
	ret += "[";
	switch(type) {
		case Token::NUMBER:
			ret += colorize(valueColor, std::to_string(ivalue)) + "]";
			break;
		case Token::CHARACTER:
			temp += "'";
			temp += cvalue;
			temp += "'";
			ret += colorize(charColor, temp) + "]";
			break;
		case Token::TRUE:
			ret += colorize(valueColor, "true") + "]";
			break;
		case Token::FALSE:
			ret += colorize(valueColor, "false") + "]";
			break;
		case Token::STRING:
			temp += "\"" + str + "\"";
			ret += colorize(stringColor, temp) + "]";
			break;
		case Token::FNUMBER:
			ret += colorize(valueColor, std::to_string(fvalue)) + "]";
			break;
	}
	return ret;
}

std::string AstNamedExpr::prettyToString() const {
	std::string ret = isLValue ? colorize(lvalueColor, "AstNamedExpr") : "AstNamedExpr";

	if(ofType) {
		ret += "{" + ofType->prettyGetTypeName() + "}";
	}
	ret += "[" + colorize(nameColor, name) + "]";
	return ret;
}

std::string AstCallExpr::prettyToString() const {
	std::string str = isLValue ? colorize(lvalueColor, "AstCallExpr") : "AstCallExpr";
	
	if(ofType) {
		str += "{" + ofType->prettyGetTypeName() + "}";
	}
	str += "[";
	str += colorize(nameColor, name) + "(";
	for (int i = 0; i < args.size(); i++) {
		str += args[i]->prettyToString();
		if (i < args.size() - 1)
			str += ", ";
	}
	str += ")]";
	return str;
}

std::string AstCastExpr::prettyToString() const {
	std::string ret = isLValue ? colorize(lvalueColor, "AstCastExpr") : "AstCastExpr";

	if(ofType) {
		ret += "{" + ofType->prettyGetTypeName() + "}";
	}
	ret += "[";
	ret += type->prettyToString() + ", " + expr->prettyToString() + "]";
	return ret;
}

std::string AstPrefixExpr::prettyToString() const {
	std::string ret = isLValue ? colorize(lvalueColor, "AstPrefixExpr") : "AstPrefixExpr";

	if(ofType) {
		ret += "{" + ofType->prettyGetTypeName() + "}";
	}
	ret += "[";

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
	ret += ", " + expr->prettyToString() + "]";
	return ret;
}

std::string AstPostfixExpr::prettyToString() const {
	std::string ret = isLValue ? colorize(lvalueColor, "AstPostfixExpr") : "AstPostfixExpr";

	if(ofType) {
		ret += "{" + ofType->prettyGetTypeName() + "}";
	}
	ret += "[";

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

	ret += expr->prettyToString(); // + "]";
	if(type == 2)
		ret += ", " + index->prettyToString();
	ret += "]";
	return ret;
}

std::string AstBinaryExpr::prettyToString() const {
	std::string ret = isLValue ? colorize(lvalueColor, "AstBinExpr") : "AstBinExpr";

	if(ofType) {
		ret += "{" + ofType->prettyGetTypeName() + "}";
	}
	ret += "[";
	ret += Token::tokenNames[(int)op] + ", " + left->prettyToString() + ", " + right->prettyToString() + "]";
	return ret;
}



std::string AstStmt::prettyToString() const {
	return "AstStmt";
}

std::string AstExprStmt::prettyToString() const {
	return "AstExprStmt[" + expr->prettyToString() + "]";
}

std::string AstAssignStmt::prettyToString() const {
	return "AstAssignStmt[" + Token::tokenNames[(int)op] + ", " + left->prettyToString() + ", " + right->prettyToString() + "]";
}

std::string AstCompStmt::prettyToString() const {
	std::string str = "AstCompStmt[";
	for (int i = 0; i < stmts.size(); i++) {
		str += stmts[i]->prettyToString();
		if (i < stmts.size() - 1)
			str += ", ";
	}
	str += "]";
	return str;
}

std::string AstIfStmt::prettyToString() const {
	return "AstIfStmt[" + cond->prettyToString() + ", " + stmt->prettyToString() + (elseStmt ? (", " + elseStmt->prettyToString()) : "") + "]";
}

std::string AstWhileStmt::prettyToString() const {
	return "AstWhileStmt[" + cond->prettyToString() + ", " + stmt->prettyToString() + "]";
}

std::string AstReturnStmt::prettyToString() const {
	std::string ret = "AstReturnStmt";
	if(ofType)
		ret += "{" + colorize(typeColor, ofType->prettyGetTypeName()) + "}";
	ret += "[" + (expr ? expr->prettyToString() : "") + "]";
	
	return ret;
}

std::string AstVarStmt::prettyToString() const {
	return "AstVarStmt[" + decl.prettyToString() + "]";
}

std::string AstFunStmt::prettyToString() const {
	return "AstFunStmt[" + decl->prettyToString() + "]";
}
