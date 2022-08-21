#include "Synan.h"

bool Synan::parse() {
	int counter = 0;
	while(pos < tokens.size()) {
		if(!isDecl()) {
			return false;
		}

		if(counter++ > 10000)
			break;
	}

	return true;
}

bool Synan::isDecl() {
	// Logger::getInstance().log("DECLS: %d\n", pos);

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
		if(isTokenType(Token::ASSIGN) && isExpr()) {

		}

		if(!isTokenType(Token::SEMICOLON)) {
			Logger::getInstance().error("Syntax error at line %d: expected ';'", tokens[pos-1].getLine());
			pos = oldPos;
			return false;
		}

		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isFunDecl() {
	int oldPos = pos;
	
	if(isType() && isTokenType(Token::IDENTIFIER) && isTokenType(Token::LPAREN) && isParDecl() && isTokenType(Token::RPAREN)) {
		if(isTokenType(Token::SEMICOLON)) {
			return true;
		}
		if(isCompoundStmt()) {
			Logger::getInstance().log("Compound stmt");
			return true;
		}
		
			// Logger::getInstance().error("Syntax error at line %d: missing function body.", tokens[pos-1].getLine());
			// pos = oldPos;
			// return false;
	}

	pos = oldPos;
	return false;
}

bool Synan::isParDecl() {
	if(isType() && isTokenType(Token::IDENTIFIER)) {
		while(isTokenType(Token::COMMA) && isType() && isTokenType(Token::IDENTIFIER)) {}
		Logger::getInstance().log("Par decl");
		return true;
	}

	return true;
}

bool Synan::isTypeDecl() {
	int oldPos = pos;

	if(isTokenType(Token::TYPEDEF) && isType() && isTokenType(Token::IDENTIFIER)) {
		if(!isTokenType(Token::SEMICOLON)) {
			Logger::getInstance().error("Syntax error at line %d: expected ';'", tokens[pos-1].getLine());
			pos = oldPos;
			return false;
		}
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isStructDecl() {
	int oldPos = pos;
	
	if(isTokenType(Token::STRUCT) && isTokenType(Token::IDENTIFIER) && isTokenType(Token::LBRACE)) {
		while(isVarDecl()) {
			Logger::getInstance().log("Var decl");
		} 
		
		if(isTokenType(Token::RBRACE)) {
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
		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isNamedType() {
	return isTokenType(Token::IDENTIFIER);
}

bool Synan::isArrayType() {
	int oldPos = pos;

	if((isAtomicType() || isNamedType()) && isTokenType(Token::LBRACKET) && isExpr() && isTokenType(Token::RBRACKET)) {
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
	if(isTokenType(Token::NUMBER) || isTokenType(Token::CHARACTER) || isTokenType(Token::STRING) || isTokenType(Token::TRUE) || isTokenType(Token::FALSE)) {
		return true;
	}

	return false;
}

bool Synan::isVariableAccess() {
	return isTokenType(Token::IDENTIFIER);
}

bool Synan::isFunctionCall() {
	int oldPos = pos;
	if(isTokenType(Token::IDENTIFIER) && isTokenType(Token::LPAREN)) {
		if(isExpr()) {
			while(isTokenType(Token::COMMA) && isExpr()) {}
		}

		if(isTokenType(Token::RPAREN)) {
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

		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isCompoundStmt() {
	int oldPos = pos;

	if(!isTokenType(Token::LBRACE))
		return false;
	
	while(isStmt());

	if(isTokenType(Token::RBRACE))
		return true;

	pos = oldPos;
	return false;
}

bool Synan::isIfStmt() {
	int oldPos = pos;
	if(isTokenType(Token::IF) && isEnclosedExpr() && isStmt()) {
		if(isTokenType(Token::ELSE)) {
			if(isStmt()) {
				return true;
			}

			return false;
		}

		return true;
	}

	pos = oldPos;
	return false;
}

bool Synan::isWhileStmt() {
	int oldPos = pos;
	if(isTokenType(Token::WHILE) && isEnclosedExpr() && isStmt()) {
		return true;
	}
	pos = oldPos;
	return false;
}

bool Synan::isReturnStmt() {
	int oldPos = pos;
	if(isTokenType(Token::RETURN)) {
		if(isExpr()) {

		}

		if(!isTokenType(Token::SEMICOLON)) {
			Logger::getInstance().error("Syntax error line %d: expected ';'", tokens[pos].getLine());
			pos = oldPos;
			return false;
		}

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