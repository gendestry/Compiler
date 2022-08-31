#include "TypeResolver.h"
#include "Ast.h"
#include "Logger.h"


bool TypeResolver::visit(AstVarDecl* varDecl, Phase phase) {
	if(!varDecl->type->accept(this, phase)) {
		return false;
	}

	if(varDecl->expr && !varDecl->expr->accept(this, phase)) {
		return false;
	}

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
	return true;
}


bool TypeResolver::visit(AstConstExpr* constExpr, Phase phase) {
	AstType::Type type;
	if(constExpr->type == Token::NUMBER) {
		type = AstType::INT;
	} else if(constExpr->type == Token::CHAR) {
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
	return true;
}

bool TypeResolver::visit(AstNamedExpr* namedExpr, Phase phase) {
	AstVarDecl* decl = (AstVarDecl*)namedExpr->declaration;
	namedExpr->ofType = decl->type;
	return true;
}

bool TypeResolver::visit(AstCallExpr* callExpr, Phase phase) {
	for(AstExpr* expr : callExpr->args) {
		if(!expr->accept(this, phase))
			return false;
	}

	AstFunDecl* decl = (AstFunDecl*)callExpr->declaration;
	callExpr->ofType = decl->type;
	return true;
}

bool TypeResolver::visit(AstCastExpr* castExpr, Phase phase) {
	if(!castExpr->expr->accept(this, phase)) {
		return false;
	}

	castExpr->ofType = castExpr->type;
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
			break;
	};

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
			break;
	};
	return true;
}


bool TypeResolver::visit(AstExprStmt* exprStmt, Phase phase) {
	if(!exprStmt->expr->accept(this, phase)) {
		return false;
	}

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

	// check for += -= *= /= %=
	if(assignStmt->op != AstAssignStmt::EQU) {
		if(assignStmt->left->ofType->type != AstType::INT || assignStmt->left->ofType->type != AstType::FLOAT) {
			Logger::getInstance().error("Type error: Invalid type for assignment %s", assignStmt->toString().c_str());
			return false;
		}
	}

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

	if(!ifStmt->elseStmt->accept(this, phase)) {
		return false;
	}

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

	return true;
}

bool TypeResolver::visit(AstReturnStmt* returnStmt, Phase phase) {
	if(returnStmt->expr && !returnStmt->expr->accept(this, phase)) {
		return false;
	}

	return true;
}

bool TypeResolver::visit(AstVarStmt* varStmt, Phase phase) {
	if(!varStmt->decl.accept(this, phase)) {
		return false;
	}

	return true;
}
