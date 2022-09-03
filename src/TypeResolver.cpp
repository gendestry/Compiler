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
			Logger::getInstance().error("Type error: Type mismatch at variable declaration %s%s!", varDecl->name.c_str(), varDecl->loc.toString().c_str());
			return false;
		}

		if(varDecl->type->type == AstType::PTR || varDecl->type->type == AstType::ARRAY) {
			AstType* left = varDecl->type;
			AstType* right =varDecl->expr->ofType;
			if(!resolvePtrOrArrType(left, right)) {
				Logger::getInstance().error("Type error: Pointer type mismatch at variable declaration %s%s!", varDecl->name.c_str(), varDecl->loc.toString().c_str());
				return false;
			}
		}
	}

	Logger::getInstance().log("Type resolved: %s%s!", varDecl->prettyToString().c_str(), varDecl->loc.toString().c_str());
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

	if(funDecl->type->type == AstType::VOID)
		funDecl->hasReturn = true;

	if(funDecl->params && !funDecl->params->accept(this, phase)) {
		return false;
	}

	if(funDecl->body && !funDecl->body->accept(this, phase)) {
		return false;
	}

	if(!funDecl->hasReturn) {
		Logger::getInstance().error("Type error: Function %s%s does not have return statement!", funDecl->name.c_str(), funDecl->loc.toString().c_str());
		return false;
	}

	return true;
}

bool TypeResolver::visit(AstTypeDecl* typeDecl, Phase phase) {
	
	return true;
}

bool TypeResolver::visit(AstStructDecl* structDecl, Phase phase) {
	std::string name = structDecl->name;
	
	for(AstVarDecl& decl : structDecl->fields) {
		if(!decl.accept(this, phase)) {
			return false;
		}
	}

	Logger::getInstance().log("Type resolved: %s%s!", structDecl->prettyToString().c_str(), structDecl->loc.toString().c_str());
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
		constExpr->ofType = new AstPtrType(constExpr->loc, new AstAtomType(constExpr->loc, type));
		return true;
	} else {
		Logger::getInstance().error("Type error: Unknown constant type %s%s!", constExpr->toString().c_str(), constExpr->loc.toString().c_str());
		return false;
	} 

	constExpr->ofType = new AstAtomType(constExpr->loc, type);

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
			Logger::getInstance().error("Type error: Function %s expects %d arguments, but %d were given %s!", decl->name.c_str(), decl->params->size(), callExpr->args.size(), callExpr->loc.toString().c_str());
			return false;
		}
		
		for(int i = 0; i < decl->params->size(); i++) {
			if(decl->params->params[i].type->type != callExpr->args[i]->ofType->type) {
				Logger::getInstance().error("Type error: Function %s was given invalid argument %s%s!", decl->name.c_str(), decl->params->params[i].name.c_str(), callExpr->loc.toString().c_str());
				return false;
			}
		}
	}
	else {
		if(callExpr->args.size() > 0) {
			Logger::getInstance().error("Type error: Function %s expects no arguments, but %d were given%s!", decl->name.c_str(), callExpr->args.size(), callExpr->loc.toString().c_str());
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
		Logger::getInstance().error("Type error: Cannot cast to void %s%s!", castExpr->toString().c_str(), castExpr->loc.toString().c_str());
		return false;
	}
	else if(castExpr->expr->ofType->type == AstType::VOID) {
		Logger::getInstance().error("Type error: Cannot cast from void %s%s!", castExpr->toString().c_str(), castExpr->loc.toString().c_str());
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
				Logger::getInstance().error("Type error: Invalid type for prefix operator (only int, float) %s%s!", prefixExpr->toString().c_str(), prefixExpr->loc.toString().c_str());
				return false;
			}
			break;
		case AstPrefixExpr::NOT:
			if(prefixExpr->expr->ofType->type == AstType::BOOL) {
				prefixExpr->ofType = prefixExpr->expr->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for prefix operator (only bool) %s%s!", prefixExpr->toString().c_str(), prefixExpr->loc.toString().c_str());
				return false;
			}
			break;
		case AstPrefixExpr::DEREF:
			if(prefixExpr->expr->ofType->type == AstType::PTR) {
				prefixExpr->ofType = ((AstPtrType*)prefixExpr->expr->ofType)->ptrType;
			}
			else {
				Logger::getInstance().error("Type error: Type must be of pointer type %s%s!", prefixExpr->toString().c_str(), prefixExpr->loc.toString().c_str());
				return false;
			}
			break;
		case AstPrefixExpr::ADDR:
			prefixExpr->ofType = new AstPtrType(prefixExpr->loc, prefixExpr->expr->ofType);
			break;
		case AstPrefixExpr::NEGATE: // ~
			if(prefixExpr->expr->ofType->type == AstType::INT) {
				prefixExpr->ofType = prefixExpr->expr->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for prefix operator (only int) %s%s!", prefixExpr->toString().c_str(), prefixExpr->loc.toString().c_str());
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
				Logger::getInstance().error("Type error: Invalid type for postfix operator %s%s!", postfixExpr->toString().c_str(), postfixExpr->loc.toString().c_str());
				return false;
			}
			break;
		case AstPostfixExpr::ACCESS: // id.id
			if(postfixExpr->expr->ofType->type == AstType::NAMED) {
				AstNamedType* namedType = (AstNamedType*)postfixExpr->expr->ofType;
				AstStructDecl* declaration = (AstStructDecl*)namedType->declaration;

				bool flag = false;
				for(AstVarDecl& field : declaration->fields) {
					if(field.name == postfixExpr->name) {
						postfixExpr->ofType = field.type;
						flag = true;
						break;
					}
				}
				
				if(!flag) {
					Logger::getInstance().error("Type error: Struct %s doesn't have field with name %s%s!", namedType->name.c_str(), postfixExpr->name.c_str(), postfixExpr->loc.toString().c_str());
					return false;
				}
			}
			else {
				Logger::getInstance().error("Type error: How did you get here? %s%s!", postfixExpr->toString().c_str(), postfixExpr->loc.toString().c_str());
				return false;
			}
			break;
		case AstPostfixExpr::PTRACCESS: // id->id
			if(postfixExpr->expr->ofType->type == AstType::PTR) {
				AstPtrType* ptrType = (AstPtrType*)postfixExpr->expr->ofType;

				if(ptrType->ptrType->type == AstType::NAMED) {
					AstNamedType* namedType = (AstNamedType*)ptrType->ptrType;
					AstStructDecl* declaration = (AstStructDecl*)namedType->declaration;
					bool flag = false;
					for(AstVarDecl& field : declaration->fields) {
						if(field.name == postfixExpr->name) {
							postfixExpr->ofType = field.type;
							flag = true;
							break;
						}
					}
					
					if(!flag) {
						Logger::getInstance().error("Type error: Struct %s doesn't have field with name %s%s!", namedType->name.c_str(), postfixExpr->name.c_str(), postfixExpr->loc.toString().c_str());
						return false;
					}
				}
				else {
					Logger::getInstance().error("Type error: Trying to access a nonexisting struct element %s%s!", postfixExpr->toString().c_str(), postfixExpr->loc.toString().c_str());
					return false;
				}
			}
			else {
				Logger::getInstance().error("Type error: Trying to access non-pointer struct with -> %s%s!", postfixExpr->toString().c_str(), postfixExpr->loc.toString().c_str());
				return false;
			}	
			break;
		case AstPostfixExpr::ARRAYACCESS: // id[id]
			if(!postfixExpr->index->accept(this, phase)) {
				return false;
			}

			if(postfixExpr->index->ofType->type != AstType::INT) {
				Logger::getInstance().error("Type error: Array index must be of type int %s%s!", postfixExpr->toString().c_str(), postfixExpr->loc.toString().c_str());
				return false;
			}

			if(postfixExpr->expr->ofType->type == AstType::ARRAY) {
				postfixExpr->ofType = ((AstArrayType*)postfixExpr->expr->ofType)->arrayType;
			}
			else if(postfixExpr->expr->ofType->type == AstType::PTR) {
				postfixExpr->ofType = ((AstPtrType*)postfixExpr->expr->ofType)->ptrType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid array type %s%s!", postfixExpr->toString().c_str(), postfixExpr->loc.toString().c_str());
				return false;
			}

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
				Logger::getInstance().error("Type error: Invalid type for binary operator %s%s!", binaryExpr->toString().c_str(), binaryExpr->loc.toString().c_str());
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
				binaryExpr->ofType = new AstAtomType(binaryExpr->loc, AstAtomType::BOOL);
			}
			else if(binaryExpr->left->ofType->type == AstType::FLOAT && binaryExpr->right->ofType->type == AstType::FLOAT) {
				binaryExpr->ofType = new AstAtomType(binaryExpr->loc, AstAtomType::BOOL);
			}
			else if(binaryExpr->left->ofType->type == AstType::PTR && binaryExpr->right->ofType->type == AstType::PTR) {
				if(!resolvePtrOrArrType(binaryExpr->left->ofType, binaryExpr->right->ofType)) {
					Logger::getInstance().error("Type error: Invalid type for binary operator %s%s!", binaryExpr->toString().c_str(), binaryExpr->loc.toString().c_str());
					return false;
				}
				binaryExpr->ofType = new AstAtomType(binaryExpr->loc, AstAtomType::BOOL);
			}
			else if(binaryExpr->left->ofType->type == AstType::CHAR && binaryExpr->right->ofType->type == AstType::CHAR) {
				binaryExpr->ofType = new AstAtomType(binaryExpr->loc, AstAtomType::BOOL);
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for binary operator %s%s!", binaryExpr->toString().c_str(), binaryExpr->loc.toString().c_str());
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
				Logger::getInstance().error("Type error: Invalid type for binary operator %s%s!", binaryExpr->toString().c_str(), binaryExpr->loc.toString().c_str());
				return false;
			}
			break;
		case AstBinaryExpr::ANDAND:
		case AstBinaryExpr::OROR:
			if(binaryExpr->left->ofType->type == AstType::BOOL && binaryExpr->right->ofType->type == AstType::BOOL) {
				binaryExpr->ofType = binaryExpr->left->ofType;
			}
			else {
				Logger::getInstance().error("Type error: Invalid type for binary operator %s%s!", binaryExpr->toString().c_str(), binaryExpr->loc.toString().c_str());
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

	Logger::getInstance().log("Type resolved: %s%s!", exprStmt->prettyToString().c_str(), exprStmt->loc.toString().c_str());
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
		Logger::getInstance().error("Type error: Invalid type for assignment %s%s!", assignStmt->toString().c_str(), assignStmt->loc.toString().c_str());
		return false;
	}

	if(assignStmt->left->ofType->type == AstType::PTR || assignStmt->left->ofType->type == AstType::ARRAY) {
		if(!resolvePtrOrArrType(assignStmt->left->ofType, assignStmt->right->ofType)) {
			Logger::getInstance().error("Type error: Invalid type for pointer assignment %s%s!", assignStmt->toString().c_str(), assignStmt->loc.toString().c_str());
			return false;
		}
	}

	// check for += -= *= /= %=
	if(assignStmt->op != AstAssignStmt::EQU) {
		if(assignStmt->left->ofType->type != AstType::INT && assignStmt->left->ofType->type != AstType::FLOAT) {
			Logger::getInstance().error("Type error: Invalid type for assignment %s%s!", assignStmt->toString().c_str(), assignStmt->loc.toString().c_str());
			return false;
		}
	}

	Logger::getInstance().log("Type resolved: %s%s!", assignStmt->prettyToString().c_str(), assignStmt->loc.toString().c_str());
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
		Logger::getInstance().error("Type error: Invalid type for if condition %s%s!", ifStmt->cond->toString().c_str(), ifStmt->cond->loc.toString().c_str());
		return false;
	}

	if(!ifStmt->stmt->accept(this, phase)) {
		return false;
	}

	if(ifStmt->elseStmt && !ifStmt->elseStmt->accept(this, phase)) {
		return false;
	}

	Logger::getInstance().log("Type resolved: %s%s!", ifStmt->prettyToString().c_str(), ifStmt->loc.toString().c_str());
	return true;
}

bool TypeResolver::visit(AstWhileStmt* whileStmt, Phase phase) {
	if(!whileStmt->cond->accept(this, phase)) {
		return false;
	}

	if(whileStmt->cond->ofType->type != AstType::BOOL) {
		Logger::getInstance().error("Type error: Invalid type for while condition %s%s!", whileStmt->cond->toString().c_str(), whileStmt->cond->loc.toString().c_str());
		return false;
	}

	if(!whileStmt->stmt->accept(this, phase)) {
		return false;
	}

	Logger::getInstance().log("Type resolved: %s%s!", whileStmt->prettyToString().c_str(), whileStmt->loc.toString().c_str());
	return true;
}

bool TypeResolver::visit(AstReturnStmt* returnStmt, Phase phase) {
	if(returnStmt->expr && !returnStmt->expr->accept(this, phase)) {
		return false;
	}

	if(returnStmt->expr) { // not void
		if(!resolvePtrOrArrType(returnStmt->expr->ofType, returnStmt->funDecl->type)) {
			Logger::getInstance().error("Type error: Type mismatch for return in function %s%s!", returnStmt->funDecl->name.c_str(), returnStmt->loc.toString().c_str());
			return false;
		}

		returnStmt->ofType = returnStmt->expr->ofType;
	}
	else {	// return void
		if(returnStmt->funDecl->type->type != AstType::VOID) {
			Logger::getInstance().error("Type error: Invalid return statement in function %s%s!", returnStmt->funDecl->name.c_str(), returnStmt->loc.toString().c_str());
			return false;
		}

		returnStmt->ofType = returnStmt->funDecl->type;
	}

	returnStmt->funDecl->hasReturn = true;

	Logger::getInstance().log("Type resolved: %s%s!", returnStmt->prettyToString().c_str(), returnStmt->loc.toString().c_str());
	return true;
}

bool TypeResolver::visit(AstVarStmt* varStmt, Phase phase) {
	if(!varStmt->decl.accept(this, phase)) {
		return false;
	}

	return true;
}

bool TypeResolver::visit(AstFunStmt* funStmt, Phase phase) {
	if(!funStmt->decl->accept(this, phase)) {
		return false;
	}

	return true;
}