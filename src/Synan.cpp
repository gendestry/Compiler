#include "Synan.h"

Synan::~Synan() {
	for (AstDecl* decl : decls)
		delete decl;
	for (AstType* type : types)
		delete type;
	for (AstExpr* expr : exprs)
		delete expr;
	for (AstStmt* stmt : stmts)
		delete stmt;
}

bool Synan::parse() {
	int counter = 0;
	while(pos < tokens.size()) {
		if(!isDecl()) {
			return false;
		}

		// std::cout << decls.back() ->toString() << std::endl;

		if(counter++ > 10000)
			break;
	}

	std::cout << "\nSyntax:" << std::endl;

	for(AstDecl* decl : decls) {
		std::cout << decl->toString() << std::endl;
	}

	return true;
}

bool Synan::isDecl() {
	if(isFunDecl()) {
		Logger::getInstance().log("Fun decl");
		return true;
	}
	else if(isVarDecl()) {
		Logger::getInstance().log("Var decl");
		return true;
	}
	else if(isTypeDecl()) {
		Logger::getInstance().log("Type decl");
		return true;
	}
	else if(isStructDecl()) {
		Logger::getInstance().log("Struct decl");
		return true;
	}
	
	return false;
}

bool Synan::isVarDecl() {
	int oldPos = pos;

	if(isType() && isTokenType(Token::IDENTIFIER)) {
		std::string name = tokens[pos - 1].getText();
		AstExpr* expr = nullptr;

		if(isTokenType(Token::ASSIGN) && isExpr()) {
			expr = exprs.back();
		}

		if(!isTokenType(Token::SEMICOLON)) {
			Logger::getInstance().error("Syntax error at line %d: expected ';'", tokens[pos-1].getLine());
			pos = oldPos;
			return false;
		}

		decls.push_back(new AstVarDecl(name, types.back(), expr));
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isFunDecl() {
	int oldPos = pos;
	
	if(isType() && isTokenType(Token::IDENTIFIER) && isTokenType(Token::LPAREN)) {
		std::string name = tokens[pos - 2].getText();
		bool parDecl = isParDecl();

		if(isTokenType(Token::RPAREN)) {
			if(isTokenType(Token::SEMICOLON)) {
				decls.push_back(new AstFunDecl(name, types.back(), (parDecl ? (AstParDecl*)decls.back() : nullptr)));
				return true;
			}
			if(isCompoundStmt()) {
				decls.push_back(new AstFunDecl(name, types.back(), (parDecl ? (AstParDecl*)decls.back() : nullptr), stmts.back()));
				Logger::getInstance().log("Compound stmt");
				return true;
			}
		}
	} 
		
			// Logger::getInstance().error("Syntax error at line %d: missing function body.", tokens[pos-1].getLine());
			// pos = oldPos;
			// return false;

	pos = oldPos;
	return false;
}

bool Synan::isParDecl() {
	if(isType() && isTokenType(Token::IDENTIFIER)) {
		std::vector<AstVarDecl> varDecls;
		varDecls.emplace_back(tokens[pos - 1].getText(), types.back());
		
		while(isTokenType(Token::COMMA) && isType() && isTokenType(Token::IDENTIFIER)) {
			varDecls.emplace_back(tokens[pos - 1].getText(), types.back());
		}

		Logger::getInstance().log("Par decl");
		decls.push_back(new AstParDecl(varDecls));
		return true;
	}

	return false;
}

bool Synan::isTypeDecl() {
	int oldPos = pos;

	if(isTokenType(Token::TYPEDEF) && isType() && isTokenType(Token::IDENTIFIER)) {
		if(!isTokenType(Token::SEMICOLON)) {
			Logger::getInstance().error("Syntax error at line %d: expected ';'", tokens[pos-1].getLine());
			pos = oldPos;
			return false;
		}

		decls.push_back(new AstTypeDecl(tokens[pos - 2].getText(), types.back()));
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isStructDecl() {
	int oldPos = pos;
	
	if(isTokenType(Token::STRUCT) && isTokenType(Token::IDENTIFIER) && isTokenType(Token::LBRACE)) {
		std::string name = tokens[pos - 2].getText();
		std::vector<AstVarDecl> fields;

		while(isVarDecl()) {
			fields.push_back(*(AstVarDecl*)decls.back());
			decls.pop_back();
		} 
		
		if(isTokenType(Token::RBRACE)) {
			decls.push_back(new AstStructDecl(name, fields));
			return true;
		}
	}

	return false;
}



bool Synan::isType() {
	if(isPointerType()) {
		Logger::getInstance().log("Pointer type");
		return true;
	}
	else if(isArrayType()) {
		Logger::getInstance().log("Array type");
		return true;
	}
	else if(isAtomicType()) {
		Logger::getInstance().log("Atomic type");
		return true;
	}
	else if(isNamedType()) {
		Logger::getInstance().log("Named type");
		return true;
	}

	return false;
}

bool Synan::isAtomicType() {
	int oldPos = pos;
	if(isTokenType(Token::CHAR) || isTokenType(Token::INT) || isTokenType(Token::VOID) || isTokenType(Token::FLOAT) || isTokenType(Token::BOOL)) {
		types.push_back(new AstAtomType((AstAtomType::Type)tokens[pos - 1].getType()));
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isNamedType() {
	if(isTokenType(Token::IDENTIFIER)) {
		types.push_back(new AstNamedType(tokens[pos - 1].getText()));
		return true;
	}

	return false;
}

bool Synan::isArrayType() {
	int oldPos = pos;

	if((isAtomicType() || isNamedType()) && isTokenType(Token::LBRACKET) && isExpr() && isTokenType(Token::RBRACKET)) {
		types.push_back(new AstArrayType(types.back(), exprs.back()));
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isPointerType() {
	int oldPos = pos;

	if(isAtomicType() || isNamedType()) {
		int counter = 0;
		while(isTokenType(Token::MULTIPLY)) {
			counter++;
		}
		if(counter > 0) {
			types.push_back(new AstPtrType(types.back(), counter));
			return true;
		}
	}

	pos = oldPos;
	return false;
}



bool Synan::isExpr() {
	if(isFunctionCall()) {
		Logger::getInstance().log("Function call");
		return true;
	}
	else if(isInfixExpr()) {
		Logger::getInstance().log("XFix expr");
		return true;
	}
	else if(isConstExpr()) {
		Logger::getInstance().log("Const expr");
		return true;
	}
	else if(isVariableAccess()) {
		Logger::getInstance().log("Variable access");
		return true;
	}
	else if(isEnclosedExpr()) {
		Logger::getInstance().log("Enclosed expr");
		return true;
	}
	else if(isCastExpr()) {
		Logger::getInstance().log("Cast expr");
		return true;
	}

	return false;
}

bool Synan::isConstExpr() {
	if(isTokenType(Token::NUMBER)) {
		exprs.push_back(new AstConstExpr(std::stoi(tokens[pos - 1].getText())));
		return true;
	}
	else if(isTokenType(Token::FNUMBER)) {
		exprs.push_back(new AstConstExpr(std::stof(tokens[pos - 1].getText())));
		return true;
	}
	else if (isTokenType(Token::CHARACTER)) {
		exprs.push_back(new AstConstExpr(tokens[pos - 1].getText()[0]));
		return true;
	}
	else if (isTokenType(Token::TRUE) || isTokenType(Token::FALSE)) {
		exprs.push_back(new AstConstExpr(tokens[pos - 1].getType() == Token::TRUE));
		return true;
	}
	else if(isTokenType(Token::STRING)) {
		exprs.push_back(new AstConstExpr(tokens[pos - 1].getText()));
		return true;
	}


	return false;
}

bool Synan::isVariableAccess() {
	if(isTokenType(Token::IDENTIFIER)) {
		exprs.push_back(new AstNamedExpr(tokens[pos - 1].getText()));
		return true;
	}

	return false;
}

bool Synan::isFunctionCall() {
	int oldPos = pos;
	if(isTokenType(Token::IDENTIFIER) && isTokenType(Token::LPAREN)) {
		std::string name = tokens[pos - 2].getText();
		std::vector<AstExpr*> args;

		if(isExpr()) {
			args.push_back(exprs.back());

			while(isTokenType(Token::COMMA) && isExpr()) { 
				args.push_back(exprs.back());
			}
		}

		if(isTokenType(Token::RPAREN)) {
			exprs.push_back(new AstCallExpr(name, args));
			return true;
		}
	}

	pos = oldPos;
	return false;
}

bool Synan::isEnclosedExpr() {
	int oldPos = pos;

	if(isTokenType(Token::LPAREN) && isExpr() && isTokenType(Token::RPAREN)) {
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isCastExpr() {
	int oldPos = pos;

	if(isTokenType(Token::LPAREN) && (isAtomicType() || isNamedType() || isPointerType()) && isTokenType(Token::RPAREN) && isExpr()) {
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isInfixExpr() {
	int oldPos = pos;

	if(isInfixA() && isInfixA_()) {
		return true;
	}	

	pos = oldPos;
	return false;
}

bool Synan::isInfixA() {
	Logger::getInstance().debug("Infix A: %d", pos);
	if(isInfixB() && isInfixB_()) {
		return true;
	}

	return false;
}

bool Synan::isInfixA_() {
	Logger::getInstance().debug("Infix A_: %d", pos);
	if(isTokenType(Token::ANDAND) || isTokenType(Token::OROR)) {
		if(isInfixA() && isInfixA_()) {
			Logger::getInstance().log("Infix expr: binbin", pos);
			// add class
			return true;
		}

		return false;
	} 

	return true;
}

bool Synan::isInfixB() {
	Logger::getInstance().debug("Infix B: %d", pos);
	if(isInfixC() && isInfixC_()) {
		return true;
	}
	return false;
}

bool Synan::isInfixB_() {
	Logger::getInstance().debug("Infix B_: %d", pos);
	if(isTokenType(Token::OR) || isTokenType(Token::AND)) {
		if(isInfixB() && isInfixB_()) {
			Logger::getInstance().log("Infix expr: bin", pos);
			// add class
			return true;
		}

		return false;
	}

	return true;
}

bool Synan::isInfixC() {
	Logger::getInstance().debug("Infix C: %d", pos);
	if(isInfixD() && isInfixD_()) {
		return true;
	}
	return false;
}

bool Synan::isInfixC_() {
	Logger::getInstance().debug("Infix C_: %d", pos);
	if(isTokenType(Token::EQUAL) || isTokenType(Token::NOT_EQUAL) || isTokenType(Token::LESS_THAN) || isTokenType(Token::LESS_THAN_EQUAL) || isTokenType(Token::GREATER_THAN) || isTokenType(Token::GREATER_THAN_EQUAL)) {
		if(isInfixC() && isInfixC_()) {
			Logger::getInstance().log("Infix expr: comp", pos);
			// add class
			return true;
		}

		return false;
	}

	return true;
}

bool Synan::isInfixD() {
	Logger::getInstance().debug("Infix D: %d", pos);
	if(isInfixE() && isInfixE_()) {
		return true;
	}
	return false;
}

bool Synan::isInfixD_() {
	Logger::getInstance().debug("Infix D_: %d", pos);
	if(isTokenType(Token::PLUS) || isTokenType(Token::MINUS) || isTokenType(Token::XOR)) {
		if(isInfixD() && isInfixD_()) {
			Logger::getInstance().log("Infix expr: add\n", pos);
			// add class
			return true;
		}

		return false;
	}

	return true;
}

bool Synan::isInfixE() {
	int oldPos = pos;

	Logger::getInstance().debug("Infix E: %d", pos);
	if(isTokenType(Token::PLUS)
		|| isTokenType(Token::MINUS)
		|| isTokenType(Token::PPLUS)
		|| isTokenType(Token::MMINUS)
		|| isTokenType(Token::NOT)
		|| isTokenType(Token::ELLIPSIS)
		|| isTokenType(Token::MULTIPLY)
		|| isTokenType(Token::AND)
		|| (isTokenType(Token::LPAREN) && isType() && isTokenType(Token::RPAREN)))
	{
		if(isInfixE()) {
			Logger::getInstance().log("Prefix expr", pos);
			return true;
		}
		
//int a = *(++a + 3);
		return false;
	}

	pos = oldPos;
	if(isInfixF()) {
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isInfixE_() {
	Logger::getInstance().debug("Infix E_: %d", pos);
	if(isTokenType(Token::MULTIPLY) || isTokenType(Token::DIVIDE) || isTokenType(Token::MODULO)) { 
		if(isInfixE() && isInfixE_()) {
			Logger::getInstance().log("Infix Expr: mul", pos);
			// add class
			return true;
		}

		return false;
	}

	return true;
}

bool Synan::isInfixF() {
	Logger::getInstance().debug("Infix F: %d", pos);
	if(isInfixG() && isInfixG_()) {
		return true;
	}
	return false;
}

bool Synan::isInfixG_() {
	Logger::getInstance().debug("Infix G_: %d", pos);
	if(isVariableAccess() || isFunctionCall() || isEnclosedExpr()) {
		if(isTokenType(Token::PPLUS) || isTokenType(Token::MMINUS)
		|| (isTokenType(Token::DOT) && isTokenType(Token::IDENTIFIER))
		|| (isTokenType(Token::MINUS) && isTokenType(Token::LESS_THAN) && isTokenType(Token::IDENTIFIER))
		|| (isTokenType(Token::LBRACKET) && isExpr() && isTokenType(Token::RBRACKET))) {
			if(isInfixG_()) {
				Logger::getInstance().log("Postfix expr", pos);
				return true;
			}
		}
		
		return false;
	}

	return true;
}

bool Synan::isInfixG() {
	Logger::getInstance().debug("Infix G: %d", pos);
	if((isConstExpr() || isVariableAccess() || isFunctionCall() || isEnclosedExpr())) {
		Logger::getInstance().log("Infix const", pos);
		return true;
	}

	return false;
}



bool Synan::isStmt() {
	if(isAssignStmt()) {
		Logger::getInstance().log("Assign stmt");
		return true;
	}
	else if(isIfStmt()) {
		Logger::getInstance().log("If stmt");
		return true;
	}
	else if(isWhileStmt()) {
		Logger::getInstance().log("While stmt");
		return true;
	}
	else if(isReturnStmt()) {
		Logger::getInstance().log("Return stmt");
		return true;
	}
	else if(isExprStmt()) {
		Logger::getInstance().log("Expr stmt");
		return true;
	}
	else if(isCompoundStmt()) {
		Logger::getInstance().log("Compound stmt");
		return true;
	}
	else if(isVarDecl()) {
		stmts.push_back(new AstVarStmt(*(AstVarDecl*)decls.back()));
		decls.pop_back();
		Logger::getInstance().log("Var decl stmt");
		return true;
	}

	return false;
}

bool Synan::isExprStmt() {
	int oldPos = pos;

	if(isExpr()) {
		if(!isTokenType(Token::SEMICOLON)) {
			Logger::getInstance().error("Syntax error line %d: expected ';'", tokens[pos].getLine());
			pos = oldPos;
			return false;
		}

		stmts.push_back(new AstExprStmt(exprs.back()));
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isAssignStmt() {
	int oldPos = pos;

	if(isExpr() && isTokenType(Token::ASSIGN) && isExpr()) {
		if(!isTokenType(Token::SEMICOLON)) {
			Logger::getInstance().error("Syntax error line %d: expected ';'", tokens[pos].getLine());
			pos = oldPos;
			return false;
		}

		stmts.push_back(new AstAssignStmt(exprs[exprs.size() - 2], exprs.back()));

		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isCompoundStmt() {
	int oldPos = pos;

	if(!isTokenType(Token::LBRACE))
		return false;
	
	std::vector<AstStmt*> cstmts;
	while(isStmt()) {
		cstmts.push_back(stmts.back());
	}

	if(isTokenType(Token::RBRACE)) {
		stmts.push_back(new AstCompStmt(cstmts));
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isIfStmt() {
	int oldPos = pos;
	if(isTokenType(Token::IF) && isEnclosedExpr()) {
		AstExpr* expr = exprs.back();

		if(isStmt()) {
			AstStmt* stmt = stmts.back();
			if(isTokenType(Token::ELSE)) {
				if(isStmt()) {
					stmts.push_back(new AstIfStmt(expr, stmt, stmts.back()));
					return true;
				}

				return false;
			}

			stmts.push_back(new AstIfStmt(expr, stmt));
			return true;
		}
	}

	pos = oldPos;
	return false;
}

bool Synan::isWhileStmt() {
	int oldPos = pos;
	if(isTokenType(Token::WHILE) && isEnclosedExpr()) {
		AstExpr* cond = exprs.back();
		if(isStmt()) {
			stmts.push_back(new AstWhileStmt(cond, stmts.back()));
			return true;
		}
	}
	pos = oldPos;
	return false;
}

bool Synan::isReturnStmt() {
	int oldPos = pos;
	if(isTokenType(Token::RETURN)) {
		AstExpr* expr = nullptr;
		if(isExpr()) {
			expr = exprs.back();
		}

		if(!isTokenType(Token::SEMICOLON)) {
			Logger::getInstance().error("Syntax error line %d: expected ';'", tokens[pos].getLine());
			pos = oldPos;
			return false;
		}

		stmts.push_back(new AstReturnStmt(expr));
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isTokenType(Token::TokenType type) { 
	// Logger::getInstance().log("TOKEN[%d]: %d\n", type, pos);
	if (tokens[pos].getType() == type) { 
		pos += 1; 
		return true;
	}

	return false;
}
