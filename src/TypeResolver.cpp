#include "TypeResolver.h"
#include "Ast.h"
#include "Logger.h"

bool TypeResolver::resolvePtrOrArrType(AstType* left, AstType* right) {
	while(true) {
		// Logger::getInstance().debug("%s %s", left->toString().c_str(), right->toString().c_str());
		if(left->type != right->type) {
			return false;
		}

		if(left->type == AstType::PTR) {
			left = ((AstPtrType*) left)->ptrType;
			right = ((AstPtrType*) right)->ptrType;
		}
		else if(left->type == AstType::ARRAY) {
			left = ((AstArrayType*) left)->arrayType;
			right = ((AstArrayType*) right)->arrayType;
		}
		else {
			break;
		}
	}

	return true;
}


bool TypeResolver::visit(AstVarDecl* varDecl, Phase phase) {
	if(!varDecl->type->accept(this, phase)) {
		return false;
	}

	if(varDecl->expr && !varDecl->expr->accept(this, phase)) {
		return false;
	}

	if(varDecl->expr) {
		if(varDecl->type->type != varDecl->expr->ofType->type) {
			Logger::getInstance().error("Type error: Type mismatch at variable declaration %s", varDecl->name.c_str());
			return false;
		}

		if(varDecl->type->type == AstType::PTR || varDecl->type->type == AstType::ARRAY) {
			AstType* left = varDecl->type;
			AstType* right =varDecl->expr->ofType;
			if(!resolvePtrOrArrType(left, right)) {
				Logger::getInstance().error("Type error: Pointer type mismatch at variable declaration %s", varDecl->name.c_str());
				return false;
			}
		}
	}

	Logger::getInstance().log("Type resolved: %s", varDecl->prettyToString().c_str());
	return true;
}

bool TypeResolver::visit(AstParDecl* parDecl, Phase phase) {
	for(AstVarDecl& decl : parDecl->params) {
		if(!decl.accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool TypeResolver::visit(AstFunDecl* funDecl, Phase phase) {
	if(!funDecl->type->accept(this, phase)) {
		return false;
	}

	if(funDecl->params && !funDecl->params->accept(this, phase)) {
		return false;
	}

	if(funDecl->body && !funDecl->body->accept(this, phase)) {
		return false;
	}
	return true;
}

bool TypeResolver::visit(AstTypeDecl* typeDecl, Phase phase) {
	return true;
}

bool TypeResolver::visit(AstStructDecl* structDecl, Phase phase) {
	return true;
}


bool TypeResolver::visit(AstAtomType* atomType, Phase phase) {
	return true;
}

bool TypeResolver::visit(AstNamedType* namedType, Phase phase) {
	return true;
}

bool TypeResolver::visit(AstPtrType* ptrType, Phase phase) {
	return true;
}

bool TypeResolver::visit(AstArrayType* arrayType, Phase phase) {
	if(!arrayType->expr->accept(this, phase)) {
		return false;
	}

	return true;
}


bool TypeResolver::visit(AstConstExpr* constExpr, Phase phase) {
	AstType::Type type;
	if(constExpr->type == Token::NUMBER) {
		type = AstType::INT;
	} else if(constExpr->type == Token::CHARACTER) {
		type = AstType::CHAR;
	} else if(constExpr->type == Token::TRUE || constExpr->type == Token::FALSE) {
		type = AstType::BOOL;
	} else if(constExpr->type == Token::FNUMBER) {
		type = AstType::FLOAT;
	} else if(constExpr->type == Token::STRING) {
		type = AstType::CHAR;
		constExpr->ofType = new AstPtrType(new AstAtomType(type));
		return true;
	} else {
		Logger::getInstance().error("Type error: Unknown constant type %s", constExpr->toString().c_str());
		return false;
	} 

	constExpr->ofType = new AstAtomType(type);

	// Logger::getInstance().log("Type resolved: %s", constExpr->toString().c_str());
	return true;
}

bool TypeResolver::visit(AstNamedExpr* namedExpr, Phase phase) {
	AstVarDecl* decl = (AstVarDecl*)namedExpr->declaration;
	namedExpr->ofType = decl->type;

	// Logger::getInstance().log("Type resolved: %s", namedExpr->toString().c_str());
	return true;
}

bool TypeResolver::visit(AstCallExpr* callExpr, Phase phase) {
	for(AstExpr* expr : callExpr->args) {
		if(!expr->accept(this, phase))
			return false;
	}

	AstFunDecl* decl = (AstFunDecl*)callExpr->declaration;

	if(decl->params) {
		if(decl->params->size() != callExpr->args.size()) {
			Logger::getInstance().error("Type error: Function %s expects %d arguments, but %d were given", decl->name.c_str(), decl->params->size(), callExpr->args.size());
			return false;
		}
		
		for(int i = 0; i < decl->params->size(); i++) {
			if(decl->params->params[i].type->type != callExpr->args[i]->ofType->type) {
				Logger::getInstance().error("Type error: Function %s was given invalid argument %s", decl->name.c_str(), decl->params->params[i].name.c_str());
				return false;
			}
		}
	}
	else {
		if(callExpr->args.size() > 0) {
			Logger::getInstance().error("Type error: Function %s expects no arguments, but %d were given", decl->name.c_str(), callExpr->args.size());
			return false;
		}
	}

	callExpr->ofType = decl->type;

	// Logger::getInstance().log("Type resolved: %s", callExpr->toString().c_str());
	return true;
}

bool TypeResolver::visit(AstCastExpr* castExpr, Phase phase) {
	if(!castExpr->expr->accept(this, phase)) {
		return false;
	}

	if(castExpr->type->type == AstType::VOID) {
		Logger::getInstance().error("Type error: Cannot cast to void %s", castExpr->toString().c_str());
		return false;
	}
	else if(castExpr->expr->ofType->type == AstType::VOID) {
		Logger::getInstance().error("Type error: Cannot cast from void %s", castExpr->toString().c_str());
		return false;
	}

	castExpr->ofType = castExpr->type;

	// Logger::getInstance().log("Type resolved: %s", castExpr->toString().c_str());
	return true;
}

bool TypeResolver::visit(AstPrefixExpr* prefixExpr, Phase phase) {
	if(!prefixExpr->expr->accept(this, phase)) {
		return false;
	}

	switch(prefixExpr->op) {
		case AstPrefixExpr::PPLUS:
		case AstPrefixExpr::MMINUS:
			if(prefixExpr->expr->ofType->type == AstType::PTR) {
				prefixExpr->ofType = prefixExpr->expr->ofType;
				return true;
			}
			else if(prefixExpr->expr->ofType->type == AstType::CHAR) {
				prefixExpr->ofType = prefixExpr->expr->ofType;
				return true;
			}
		case AstPrefixExpr::PLUS:
		case AstPrefixExpr::MINUS:
			if(prefixExpr->expr->ofType->type == AstType::INT) {
				prefixExpr->ofType = prefixExpr->expr->ofType;
			}
			else if(prefixExpr->expr->ofType->type == AstType::FLOAT) {
				prefixExpr->ofType = prefixExpr->expr->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for prefix operator %s", prefixExpr->toString().c_str());
				return false;
			}
			break;
		case AstPrefixExpr::NOT:
			if(prefixExpr->expr->ofType->type == AstType::BOOL) {
				prefixExpr->ofType = prefixExpr->expr->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for prefix operator %s", prefixExpr->toString().c_str());
				return false;
			}
			break;
		case AstPrefixExpr::DEREF:
			if(prefixExpr->expr->ofType->type == AstType::PTR) {
				prefixExpr->ofType = ((AstPtrType*)prefixExpr->expr->ofType)->ptrType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for prefix operator %s", prefixExpr->toString().c_str());
				return false;
			}
			break;
		case AstPrefixExpr::ADDR:
			prefixExpr->ofType = new AstPtrType(prefixExpr->expr->ofType);
			break;
		case AstPrefixExpr::NEGATE: // ~
			if(prefixExpr->expr->ofType->type == AstType::INT) {
				prefixExpr->ofType = prefixExpr->expr->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for prefix operator %s", prefixExpr->toString().c_str());
				return false;
			}
			break;
		
	};

	// Logger::getInstance().log("Type resolved: %s", prefixExpr->toString().c_str());
	return true;
}

bool TypeResolver::visit(AstPostfixExpr* postfixExpr, Phase phase) {
	if(!postfixExpr->expr->accept(this, phase)) {
		return false;
	}

	switch(postfixExpr->op) {
		case AstPostfixExpr::PPLUS:
		case AstPostfixExpr::MMINUS:
			if(postfixExpr->expr->ofType->type == AstType::INT) {
				postfixExpr->ofType = postfixExpr->expr->ofType;
			}
			else if(postfixExpr->expr->ofType->type == AstType::FLOAT) {
				postfixExpr->ofType = postfixExpr->expr->ofType;
			}
			else if(postfixExpr->expr->ofType->type == AstType::PTR) {
				postfixExpr->ofType = postfixExpr->expr->ofType;
			}
			else if(postfixExpr->expr->ofType->type == AstType::CHAR) {
				postfixExpr->ofType = postfixExpr->expr->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for postfix operator %s", postfixExpr->toString().c_str());
				return false;
			}
			break;
		case AstPostfixExpr::ACCESS: // id.id
			break;
		case AstPostfixExpr::PTRACCESS: // id->id
			break;
		case AstPostfixExpr::ARRAYACCESS: // id[id]
			if(!postfixExpr->index->accept(this, phase)) {
				return false;
			}

			AstNamedExpr* namedExpr = (AstNamedExpr*)postfixExpr->expr;
			AstVarDecl* decl = (AstVarDecl*)namedExpr->declaration;

			if(postfixExpr->index->ofType->type != AstType::INT) {
				Logger::getInstance().error("Type error: Array index must be of type int %s: %s", namedExpr->name.c_str(), postfixExpr->toString().c_str());
				return false;
			}

			if(decl->type->type == AstType::ARRAY) {
				postfixExpr->expr->ofType = ((AstArrayType*)decl->type)->arrayType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid array type %s", postfixExpr->toString().c_str());
				return false;
			}
			break;
	};

	// Logger::getInstance().log("Type resolved: %s", postfixExpr->toString().c_str());
	return true;
}

bool TypeResolver::visit(AstBinaryExpr* binaryExpr, Phase phase) {
	if(!binaryExpr->left->accept(this, phase)) {
		return false;
	}

	if(!binaryExpr->right->accept(this, phase)) {
		return false;
	}

	switch(binaryExpr->op) {
		case AstBinaryExpr::PLUS:
		case AstBinaryExpr::MINUS:
		case AstBinaryExpr::MUL:
		case AstBinaryExpr::DIV:
		case AstBinaryExpr::MOD:
			if(binaryExpr->left->ofType->type == AstType::INT && binaryExpr->right->ofType->type == AstType::INT) {
				binaryExpr->ofType = binaryExpr->left->ofType;
			}
			else if(binaryExpr->left->ofType->type == AstType::FLOAT && binaryExpr->right->ofType->type == AstType::FLOAT) {
				binaryExpr->ofType = binaryExpr->left->ofType;
			}
			else if(binaryExpr->left->ofType->type == AstType::PTR && binaryExpr->right->ofType->type == AstType::INT) {
				binaryExpr->ofType = binaryExpr->left->ofType;
			}
			else if(binaryExpr->left->ofType->type == AstType::INT && binaryExpr->right->ofType->type == AstType::PTR) {
				binaryExpr->ofType = binaryExpr->right->ofType;
			}
			else if(binaryExpr->left->ofType->type == AstType::CHAR && binaryExpr->right->ofType->type == AstType::INT) {
				binaryExpr->ofType = binaryExpr->left->ofType;
			}
			else if(binaryExpr->left->ofType->type == AstType::INT && binaryExpr->right->ofType->type == AstType::CHAR) {
				binaryExpr->ofType = binaryExpr->right->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for binary operator %s", binaryExpr->toString().c_str());
				return false;
			}
			break;
		case AstBinaryExpr::EQU:
		case AstBinaryExpr::NEQ:
		case AstBinaryExpr::LESS:
		case AstBinaryExpr::LESS_EQU:
		case AstBinaryExpr::GREATER:
		case AstBinaryExpr::GREATER_EQU:
			if(binaryExpr->left->ofType->type == AstType::INT && binaryExpr->right->ofType->type == AstType::INT) {
				binaryExpr->ofType = new AstAtomType(AstAtomType::BOOL);
			}
			else if(binaryExpr->left->ofType->type == AstType::FLOAT && binaryExpr->right->ofType->type == AstType::FLOAT) {
				binaryExpr->ofType = new AstAtomType(AstAtomType::BOOL);
			}
			else if(binaryExpr->left->ofType->type == AstType::PTR && binaryExpr->right->ofType->type == AstType::PTR) {
				if(!resolvePtrOrArrType(binaryExpr->left->ofType, binaryExpr->right->ofType)) {
					Logger::getInstance().error("Type error: Invalid type for binary operator %s", binaryExpr->toString().c_str());
					return false;
				}
				binaryExpr->ofType = new AstAtomType(AstAtomType::BOOL);
			}
			else if(binaryExpr->left->ofType->type == AstType::CHAR && binaryExpr->right->ofType->type == AstType::CHAR) {
				binaryExpr->ofType = new AstAtomType(AstAtomType::BOOL);
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for binary operator %s", binaryExpr->toString().c_str());
				return false;
			}
			break;
		case AstBinaryExpr::AND:
		case AstBinaryExpr::OR:
		case AstBinaryExpr::XOR:
			if(binaryExpr->left->ofType->type == AstType::INT && binaryExpr->right->ofType->type == AstType::INT) {
				binaryExpr->ofType = binaryExpr->left->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for binary operator %s", binaryExpr->toString().c_str());
				return false;
			}
			break;
		case AstBinaryExpr::ANDAND:
		case AstBinaryExpr::OROR:
			if(binaryExpr->left->ofType->type == AstType::BOOL && binaryExpr->right->ofType->type == AstType::BOOL) {
				binaryExpr->ofType = binaryExpr->left->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for binary operator %s", binaryExpr->toString().c_str());
				return false;
			}
			break;
	};

	// Logger::getInstance().log("Type resolved: %s", binaryExpr->toString().c_str());
	return true;
}


bool TypeResolver::visit(AstExprStmt* exprStmt, Phase phase) {
	if(!exprStmt->expr->accept(this, phase)) {
		return false;
	}

	Logger::getInstance().log("Type resolved: %s", exprStmt->prettyToString().c_str());
	return true;
}

bool TypeResolver::visit(AstAssignStmt* assignStmt, Phase phase) {
	// a += 3 -> a must be int and 3 must be int
	
	if(!assignStmt->left->accept(this, phase)) {
		return false;
	}

	if(!assignStmt->right->accept(this, phase)) {
		return false;
	}

	// left and right must match
	if(assignStmt->left->ofType->type != assignStmt->right->ofType->type) {
		Logger::getInstance().error("Type error: Invalid type for assignment %s", assignStmt->toString().c_str());
		return false;
	}

	if(assignStmt->left->ofType->type == AstType::PTR || assignStmt->left->ofType->type == AstType::ARRAY) {
		if(!resolvePtrOrArrType(assignStmt->left->ofType, assignStmt->right->ofType)) {
			Logger::getInstance().error("Type error: Invalid type for pointer assignment %s", assignStmt->toString().c_str());
			return false;
		}
	}

	// check for += -= *= /= %=
	if(assignStmt->op != AstAssignStmt::EQU) {
		if(assignStmt->left->ofType->type != AstType::INT && assignStmt->left->ofType->type != AstType::FLOAT) {
			Logger::getInstance().error("Type error: Invalid type for assignment %s", assignStmt->toString().c_str());
			return false;
		}
	}

	Logger::getInstance().log("Type resolved: %s", assignStmt->prettyToString().c_str());
	return true;
}

bool TypeResolver::visit(AstCompStmt* compStmt, Phase phase) {
	for(AstStmt* stmt : compStmt->stmts) {
		if(!stmt->accept(this, phase)) {
			return false;
		}
	}

	return true;
}

bool TypeResolver::visit(AstIfStmt* ifStmt, Phase phase) {
	if(!ifStmt->cond->accept(this, phase)) {
		return false;
	}

	if(ifStmt->cond->ofType->type != AstType::BOOL) {
		Logger::getInstance().error("Type error: Invalid type for if condition %s", ifStmt->cond->toString().c_str());
		return false;
	}

	if(!ifStmt->stmt->accept(this, phase)) {
		return false;
	}

	if(ifStmt->elseStmt && !ifStmt->elseStmt->accept(this, phase)) {
		return false;
	}

	Logger::getInstance().log("Type resolved: %s", ifStmt->prettyToString().c_str());
	return true;
}

bool TypeResolver::visit(AstWhileStmt* whileStmt, Phase phase) {
	if(!whileStmt->cond->accept(this, phase)) {
		return false;
	}

	if(whileStmt->cond->ofType->type != AstType::BOOL) {
		Logger::getInstance().error("Type error: Invalid type for while condition %s", whileStmt->cond->toString().c_str());
		return false;
	}

	if(!whileStmt->stmt->accept(this, phase)) {
		return false;
	}

	Logger::getInstance().log("Type resolved: %s", whileStmt->prettyToString().c_str());
	return true;
}

bool TypeResolver::visit(AstReturnStmt* returnStmt, Phase phase) {
	if(returnStmt->expr && !returnStmt->expr->accept(this, phase)) {
		return false;
	}

	if(returnStmt->expr) { // not void
		if(!resolvePtrOrArrType(returnStmt->expr->ofType, returnStmt->funDecl->type)) {
			Logger::getInstance().error("Type error: Type mismatch for return in function %s", returnStmt->funDecl->name.c_str());
			return false;
		}

		returnStmt->ofType = returnStmt->expr->ofType;
	}
	else {	// return void
		if(returnStmt->funDecl->type->type != AstType::VOID) {
			Logger::getInstance().error("Type error: Invalid return statement in function %s", returnStmt->funDecl->name.c_str());
			return false;
		}

		returnStmt->ofType = returnStmt->funDecl->type;
	}

	Logger::getInstance().log("Type resolved: %s", returnStmt->prettyToString().c_str());
	return true;
}

bool TypeResolver::visit(AstVarStmt* varStmt, Phase phase) {
	if(!varStmt->decl.accept(this, phase)) {
		return false;
	}

	// Logger::getInstance().log("Type resolved: %s", varStmt->prettyToString().c_str());
	return true;
}
