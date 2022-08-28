#pragma once
#include "Visitor.h"

class TypeResolver : public Visitor {
	// template<typename T, typename... Args>
	// bool checkType(SemType* type, T current, Args... args) {
	// 	return (type->getType() == current) || checkType(type, args...);
	// }

	// template<typename T>
	// bool checkType(SemType* type, T last) {
	// 	return type->getType() == last;
	// }

public:
	bool visit(AstVarDecl* varDecl, Phase phase) override;
	bool visit(AstParDecl* parDecl, Phase phase) override;
	bool visit(AstFunDecl* funDecl, Phase phase) override;
	bool visit(AstTypeDecl* typeDecl, Phase phase) override;
	bool visit(AstStructDecl* structDecl, Phase phase) override;

	bool visit(AstAtomType* atomType, Phase phase) override;
	bool visit(AstNamedType* namedType, Phase phase) override;
	bool visit(AstPtrType* ptrType, Phase phase) override;
	bool visit(AstArrayType* arrayType, Phase phase) override;
	
	bool visit(AstConstExpr* constExpr, Phase phase) override;
	bool visit(AstNamedExpr* namedExpr, Phase phase) override;
	bool visit(AstCallExpr* callExpr, Phase phase) override;
	bool visit(AstCastExpr* castExpr, Phase phase) override;
	bool visit(AstPrefixExpr* prefixExpr, Phase phase) override;
	bool visit(AstPostfixExpr* postfixExpr, Phase phase) override;
	bool visit(AstBinaryExpr* binaryExpr, Phase phase) override;

	bool visit(AstExprStmt* exprStmt, Phase phase) override;
	bool visit(AstAssignStmt* assignStmt, Phase phase) override;
	bool visit(AstCompStmt* compStmt, Phase phase) override;
	bool visit(AstIfStmt* ifStmt, Phase phase) override;
	bool visit(AstWhileStmt* whileStmt, Phase phase) override;
	bool visit(AstReturnStmt* returnStmt, Phase phase) override;
	bool visit(AstVarStmt* varStmt, Phase phase) override;
};