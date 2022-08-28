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
	while(pos < tokens.size()) {
		if(!isDecl()) {
			Logger::getInstance().error("Syntax error at line %d[%d].", tokens[pos].getLine(), tokens[pos].getStart());
			return false;
		}
	}

	return true;
}

void Synan::printDecls() {
	for(AstDecl* decl : decls) {
		std::cout << decl->toString() << std::endl;
	}
}

bool Synan::isDecl() {
	if(isFunDecl()) {
		Logger::getInstance().debug("Fun decl");
		return true;
	}
	else if(isVarDecl()) {
		Logger::getInstance().debug("Var decl");
		return true;
	}
	else if(isTypeDecl()) {
		Logger::getInstance().debug("Type decl");
		return true;
	}
	else if(isStructDecl()) {
		Logger::getInstance().debug("Struct decl");
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
		AstType* type = types.back();
		bool parDecl = isParDecl();

		if(isTokenType(Token::RPAREN)) {
			if(isTokenType(Token::SEMICOLON)) {
				AstParDecl* pDecl = nullptr;
				if(parDecl) {
					pDecl = (AstParDecl*)decls.back();
					decls.pop_back();
				}
				
				decls.push_back(new AstFunDecl(name, type, pDecl));
				return true;
			}
			if(isCompoundStmt()) {
				AstParDecl* pDecl = nullptr;
				if(parDecl) {
					pDecl = (AstParDecl*)decls.back();
					decls.pop_back();
				}
				
				decls.push_back(new AstFunDecl(name, type, pDecl, stmts.back()));
				Logger::getInstance().debug("Compound stmt");
				return true;
			}
		}
	} 

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

		Logger::getInstance().debug("Par decl");
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
		Logger::getInstance().debug("Pointer type");
		return true;
	}
	else if(isArrayType()) {
		Logger::getInstance().debug("Array type");
		return true;
	}
	else if(isAtomicType()) {
		Logger::getInstance().debug("Atomic type");
		return true;
	}
	else if(isNamedType()) {
		Logger::getInstance().debug("Named type");
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
		Logger::getInstance().debug("Function call");
		return true;
	}
	else if(isInfixExpr()) {
		Logger::getInstance().debug("XFix expr");
		return true;
	}
	else if(isConstExpr()) {
		Logger::getInstance().debug("Const expr");
		return true;
	}
	else if(isVariableAccess()) {
		Logger::getInstance().debug("Variable access");
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
		Token::TokenType type = tokens[pos - 1].getType();
		AstExpr* expr = exprs.back();
		if(isInfixA()) {
			Logger::getInstance().debug("Infix expr: binbin", pos);
			exprs.push_back(new AstBinaryExpr((AstBinaryExpr::Binary)type, expr, exprs.back()));
			isInfixA_();
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
	if(isTokenType(Token::OR) || isTokenType(Token::AND) || isTokenType(Token::XOR)) {
		Token::TokenType type = tokens[pos - 1].getType();
		AstExpr* expr = exprs.back();
		if(isInfixB()) {
			Logger::getInstance().debug("Infix expr: bin", pos);
			exprs.push_back(new AstBinaryExpr((AstBinaryExpr::Binary)type, expr, exprs.back()));
			isInfixB_();
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
		Token::TokenType type = tokens[pos - 1].getType();
		AstExpr* expr = exprs.back();
		if(isInfixC()) {
			Logger::getInstance().debug("Infix expr: comp", pos);
			exprs.push_back(new AstBinaryExpr((AstBinaryExpr::Binary)type, expr, exprs.back()));
			isInfixC_();
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
	if(isTokenType(Token::PLUS) || isTokenType(Token::MINUS)) {
		Token::TokenType type = tokens[pos - 1].getType();
		AstExpr* expr = exprs.back();
		if(isInfixD()) {
			Logger::getInstance().debug("Infix expr: add\n", pos);
			exprs.push_back(new AstBinaryExpr((AstBinaryExpr::Binary)type, expr, exprs.back()));
			isInfixD_();
			return true;
		}

		return false;
	}

	return true;
}

bool Synan::isInfixE() {
	int oldPos = pos;

	Logger::getInstance().debug("Infix E: %d", pos);
	bool prefix = false;
	if(isTokenType(Token::PLUS)
		|| isTokenType(Token::MINUS)
		|| isTokenType(Token::PPLUS)
		|| isTokenType(Token::MMINUS)
		|| isTokenType(Token::NOT)
		|| isTokenType(Token::ELLIPSIS)
		|| isTokenType(Token::MULTIPLY)
		|| isTokenType(Token::AND)) 
	{
		Token::TokenType type = tokens[pos - 1].getType();
		if(isInfixE()) {
			AstExpr* expr = exprs.back();
			exprs.pop_back();
			exprs.push_back(new AstPrefixExpr((AstPrefixExpr::Prefix)type, expr));

			Logger::getInstance().debug("Prefix expr", pos);
			return true;
		}
	} else if (isTokenType(Token::LPAREN) && isType() && isTokenType(Token::RPAREN)){
		AstType* type = types.back();

		if(isInfixE()) {
			AstExpr* expr = exprs.back();
			exprs.push_back(new AstCastExpr(type, expr));
			Logger::getInstance().debug("Cast expr", pos);
			return true;
		}		
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
		Token::TokenType type = tokens[pos - 1].getType();
		AstExpr* expr = exprs.back();
		if(isInfixE()) {
			Logger::getInstance().debug("Infix Expr: mul", pos);
			exprs.push_back(new AstBinaryExpr((AstBinaryExpr::Binary)type, expr, exprs.back()));
			isInfixE_();
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
	AstExpr* expr = exprs.back();

	if((isTokenType(Token::PPLUS) || isTokenType(Token::MMINUS))) {
		exprs.push_back(new AstPostfixExpr((AstPostfixExpr::Postfix)tokens[pos - 1].getType(), expr));
		isInfixG_();
	}
	else if((isTokenType(Token::DOT) && isTokenType(Token::IDENTIFIER))) {
		std::string name = tokens[pos - 1].getText();
		exprs.push_back(new AstPostfixExpr((AstPostfixExpr::Postfix)tokens[pos - 2].getType(), expr, name));
		isInfixG_();
	}
	else if((isTokenType(Token::PTR) && isTokenType(Token::IDENTIFIER))) {
		std::string name = tokens[pos - 1].getText();
		exprs.push_back(new AstPostfixExpr((AstPostfixExpr::Postfix)tokens[pos - 2].getType(), expr, name));
		isInfixG_();
	}
	else if(isTokenType(Token::LBRACKET) && isExpr() && isTokenType(Token::RBRACKET)) {
		AstExpr* index = exprs.back();
		exprs.push_back(new AstPostfixExpr(AstPostfixExpr::ARRAYACCESS, expr, index));
		isInfixG_();
	}
	
	return true;

}

bool Synan::isInfixG() {
	Logger::getInstance().debug("Infix G: %d", pos);

	if((isConstExpr() || isVariableAccess() || isFunctionCall() || isEnclosedExpr())) {
		Logger::getInstance().debug("Infix const", pos);
		return true;
	}

	return false;
}



bool Synan::isStmt() {
	if(isAssignStmt()) {
		Logger::getInstance().debug("Assign stmt");
		return true;
	}
	else if(isIfStmt()) {
		Logger::getInstance().debug("If stmt");
		return true;
	}
	else if(isWhileStmt()) {
		Logger::getInstance().debug("While stmt");
		return true;
	}
	else if(isReturnStmt()) {
		Logger::getInstance().debug("Return stmt");
		return true;
	}
	else if(isVarDecl()) {
		stmts.push_back(new AstVarStmt(*(AstVarDecl*)decls.back()));
		decls.pop_back();
		Logger::getInstance().debug("Var decl stmt");
		return true;
	}
	else if(isExprStmt()) {
		Logger::getInstance().debug("Expr stmt");
		return true;
	}
	else if(isCompoundStmt()) {
		Logger::getInstance().debug("Compound stmt");
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

	if(isExpr()) {// && isTokenType(Token::ASSIGN) && isExpr()) {
		AstExpr* left = exprs.back();
		if(isTokenType(Token::ASSIGN) || isTokenType(Token::PLUSEQU) || isTokenType(Token::MINUSEQU) || isTokenType(Token::MULTEQU) || isTokenType(Token::DIVEQU) || isTokenType(Token::MODEQU)) {
			Token::TokenType type = tokens[pos - 1].getType();
			if(isExpr()) {
				AstExpr* right = exprs.back();
				if(!isTokenType(Token::SEMICOLON)) {
					Logger::getInstance().error("Syntax error line %d: expected ';'", tokens[pos].getLine());
					pos = oldPos;
					return false;
				}

				stmts.push_back(new AstAssignStmt(left, right, (AstAssignStmt::Assign)type));
				return true;
			}
		}
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
	// Logger::getInstance().debug("TOKEN[%d]: %d\n", type, pos);
	if (tokens[pos].getType() == type) { 
		pos += 1; 
		return true;
	}

	return false;
}
