#pragma once
#include <string>
#include <vector>

class AstDecl;
class AstVarDecl;
class AstParDecl;
class AstFunDecl;
class AstTypeDecl;
class AstStructDecl;

class AstType;
class AstAtomType;
class AstNamedType;
class AstPtrType;
class AstArrayType;

class AstConstExpr;
class AstNamedExpr;
class AstCallExpr;
class AstCastExpr;
class AstPrefixExpr;
class AstPostfixExpr;
class AstBinaryExpr;

class AstExprStmt;
class AstAssignStmt;
class AstCompStmt;
class AstIfStmt;
class AstWhileStmt;
class AstReturnStmt;
class AstVarStmt;
class AstFunStmt;

struct Symb {
	std::string name;
	bool isType = false;
	int depth = 0;
	AstDecl* decl;
};

enum Phase {
	HEAD,
	BODY
};

class Visitor {
public:
	virtual bool visit(AstVarDecl* varDecl, Phase phase) = 0;
	virtual bool visit(AstParDecl* parDecl, Phase phase) = 0;
	virtual bool visit(AstFunDecl* funDecl, Phase phase) = 0;
	virtual bool visit(AstTypeDecl* typeDecl, Phase phase) = 0;
	virtual bool visit(AstStructDecl* structDecl, Phase phase) = 0;

	virtual bool visit(AstAtomType* atomType, Phase phase) = 0;
	virtual bool visit(AstNamedType* namedType, Phase phase) = 0;
	virtual bool visit(AstPtrType* ptrType, Phase phase) = 0;
	virtual bool visit(AstArrayType* arrayType, Phase phase) = 0;
	
	virtual bool visit(AstConstExpr* constExpr, Phase phase) = 0;
	virtual bool visit(AstNamedExpr* namedExpr, Phase phase) = 0;
	virtual bool visit(AstCallExpr* callExpr, Phase phase) = 0;
	virtual bool visit(AstCastExpr* castExpr, Phase phase) = 0;
	virtual bool visit(AstPrefixExpr* prefixExpr, Phase phase) = 0;
	virtual bool visit(AstPostfixExpr* postfixExpr, Phase phase) = 0;
	virtual bool visit(AstBinaryExpr* binaryExpr, Phase phase) = 0;

	virtual bool visit(AstExprStmt* exprStmt, Phase phase) = 0;
	virtual bool visit(AstAssignStmt* assignStmt, Phase phase) = 0;
	virtual bool visit(AstCompStmt* compStmt, Phase phase) = 0;
	virtual bool visit(AstIfStmt* ifStmt, Phase phase) = 0;
	virtual bool visit(AstWhileStmt* whileStmt, Phase phase) = 0;
	virtual bool visit(AstReturnStmt* returnStmt, Phase phase) = 0;
	virtual bool visit(AstVarStmt* varStmt, Phase phase) = 0;
	virtual bool visit(AstFunStmt* funStmt, Phase phase) = 0;
};
