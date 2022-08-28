#include "TypeResolver.h"
#include "Ast.h"
#include "Logger.h"


bool TypeResolver::visit(AstVarDecl* varDecl, Phase phase) {
	return true;
}

bool TypeResolver::visit(AstParDecl* parDecl, Phase phase) {
	return true;
}

bool TypeResolver::visit(AstFunDecl* funDecl, Phase phase) {
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
		case AstPrefixExpr::PLUS:
		case AstPrefixExpr::MINUS:
			if(prefixExpr->expr->ofType->type == AstType::INT) {
				prefixExpr->ofType = new AstAtomType(AstType::INT);
			}
			else if(prefixExpr->expr->ofType->type == AstType::FLOAT) {
				prefixExpr->ofType = new AstAtomType(AstType::FLOAT);
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for prefix operator %s", prefixExpr->toString().c_str());
				return false;
			}
			break;
		case AstPrefixExpr::NOT:
			if(prefixExpr->expr->ofType->type == AstType::BOOL) {
				prefixExpr->ofType = new AstAtomType(AstType::BOOL);
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
				prefixExpr->ofType = new AstAtomType(AstType::INT);
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
	return true;
}

bool TypeResolver::visit(AstBinaryExpr* binaryExpr, Phase phase) {
	
	return true;
}


bool TypeResolver::visit(AstExprStmt* exprStmt, Phase phase) {
	if(!exprStmt->accept(this, phase)) {
		return false;
	}

	return true;
}

bool TypeResolver::visit(AstAssignStmt* assignStmt, Phase phase) {
	// a += 3 -> a must be in and 3 must be int
	
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
