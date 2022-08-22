#pragma once
#include <vector>
#include "Lexan.h"
#include "Logger.h"
#include "Ast.h"


class Synan {
private:
	const std::vector<Token>& tokens;
	// std::vector<Ast*> ast;
	int pos = 0;
public:
	Synan(Lexan& lexan) : tokens(lexan.getTokens()) {
		Logger::getInstance().log("Phase 2: Syntax analysis");
	}

	// ~Synan() {
	// 	for(auto& ast : this->ast)
	// 		delete ast;
	// }

	bool parse();
	

private:
	bool isDecl();
	bool isVarDecl();
	bool isFunDecl();
	bool isParDecl();
	bool isTypeDecl();
	bool isStructDecl();

	bool isType();
	bool isAtomicType();
	bool isNamedType();
	bool isArrayType();
	bool isPointerType();

	bool isExpr();
	bool isConstExpr();
	bool isVariableAccess();
	bool isFunctionCall();
	bool isEnclosedExpr();
	bool isCastExpr();
	bool isInfixExpr();

	bool isInfixA();
	bool isInfixA_();
	bool isInfixB();
	bool isInfixB_();
	bool isInfixC();
	bool isInfixC_();
	bool isInfixD();
	bool isInfixD_();
	bool isInfixE();
	bool isInfixE_();
	bool isInfixF();
	bool isInfixG();
	bool isInfixG_();

	bool isStmt();
	bool isExprStmt();
	bool isAssignStmt();
	bool isCompoundStmt();
	bool isIfStmt();
	bool isWhileStmt();
	bool isForStmt();
	bool isBreakStmt();
	bool isReturnStmt();

	bool isTokenType(Token::TokenType type);
};



