#pragma once
#include <vector>
#include <memory>
#include "Lexan.h"
#include "Logger.h"
#include "Ast.h"


class Synan {
private:
	const std::vector<Token>& tokens;
	int pos = 0;							// current token position
	AstFunDecl* currentFunction = nullptr;	// current function

	std::vector<AstDecl*> decls;
	std::vector<AstType*> types;
	std::vector<AstExpr*> exprs;
	std::vector<AstStmt*> stmts;
public:
	Synan(Lexan& lexan) : tokens(lexan.getTokens()) {
		Logger::getInstance().log("#i#grnPhase 2: Syntax analysis#r\n");
	}

	~Synan();
	bool parse();
	void printDecls();

	std::vector<AstDecl*>& getDecls() {
		return decls;
	}

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
	bool isPtrOrArrType();

	bool isExpr();
	bool isConstExpr();
	bool isVariableAccess();
	bool isFunctionCall();
	bool isEnclosedExpr();
	bool isInPlaceExpr();
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
