#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Token.h"
#include "Visitor.h"

class AstDecl;
class AstType;
class AstExpr;
class AstStmt;


class Ast {
public:
	virtual bool accept(Visitor* visitor, Phase phase) = 0;
	
	virtual std::string toString() const = 0;
	virtual std::string prettyToString() const = 0;
};

/* ----- DECLS ----- */

class AstDecl : public Ast {
public:
public:
	virtual std::string toString() const override;
	virtual std::string prettyToString() const override;
};

class AstVarDecl : public AstDecl {
public:
	AstVarDecl(std::string name, AstType* type, AstExpr* expr = nullptr) : name(name), type(type), expr(expr) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }
	
	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstType* type = nullptr;
	AstExpr* expr = nullptr;
	std::string name;
};

class AstParDecl : public AstDecl {
public:
	AstParDecl(std::vector<AstVarDecl>params) : params(params) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
	int size() const { return params.size(); }
public:
	std::vector<AstVarDecl> params;
};

class AstFunDecl : public AstDecl {
public:
	AstFunDecl(std::string name, AstType* type, AstParDecl* params = nullptr, AstStmt* body = nullptr) : name(name), type(type), params(params), body(body) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::string name;
	AstType* type;
	AstParDecl* params;
	AstStmt* body;
};

class AstTypeDecl : public AstDecl {
public:
	AstTypeDecl(std::string name, AstType* type) : name(name), type(type) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstType* type = nullptr;
	std::string name;
};

class AstStructDecl : public AstDecl {
public:
	AstStructDecl(std::string name, std::vector<AstVarDecl> fields) : name(name), fields(fields) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::string name;
	std::vector<AstVarDecl> fields;
};

/* ----- TYPES ----- */

class AstType : public Ast {
public:
	enum Type {
		INT,
		CHAR,
		BOOL,
		VOID,
		FLOAT,
		NAMED,
		PTR,
		ARRAY
	};
public:
	virtual std::string toString() const override;
	virtual std::string prettyToString() const override;
	std::string getTypeName() const;
	std::string prettyGetTypeName() const;

	Type type;
};

class AstAtomType : public AstType {
public:
	AstAtomType(Type atomType) { type = atomType; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
};

class AstNamedType : public AstType {
public:
	AstNamedType(std::string name) : name(name) { type = NAMED; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::string name;
	AstDecl* declaration = nullptr;
};

class AstPtrType : public AstType {
public:
	AstPtrType(AstType* ptrType) : ptrType(ptrType) { type = PTR; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstType* ptrType;
};

class AstArrayType : public AstType {
public:
	AstArrayType(AstType* arrayType, AstExpr* expr) : arrayType(arrayType), expr(expr) { type = ARRAY; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstType* arrayType;
	AstExpr* expr;
};

/* ----- EXPRS ----- */

class AstExpr : public Ast {
public:
	virtual std::string toString() const override;
	virtual std::string prettyToString() const override;

	AstType* ofType = nullptr;
};

class AstConstExpr : public AstExpr {
public:
	AstConstExpr(int value) : ivalue(value) { type = Token::NUMBER; }
	AstConstExpr(char value) : cvalue(value) { type = Token::CHARACTER; }
	AstConstExpr(bool value) : bvalue(value) { type = (value ? Token::TRUE : Token::FALSE); }
	AstConstExpr(float value) : fvalue(value) { type = Token::FNUMBER; }
	AstConstExpr(std::string value) : str(value) { type = Token::STRING;}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	Token::TokenType type;
	union {
		float fvalue;
		int ivalue;
		char cvalue;
		bool bvalue;
	};
	std::string str;
};

class AstNamedExpr : public AstExpr {
public:
	AstNamedExpr(std::string name) : name(name) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::string name;
	AstDecl* declaration = nullptr;
};

class AstCallExpr : public AstExpr {
public:
	AstCallExpr(std::string name, std::vector<AstExpr*> args) : name(name), args(args) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::string name;
	std::vector<AstExpr*> args;
	AstDecl* declaration;
};

class AstCastExpr : public AstExpr {
public:
	AstCastExpr(AstType* type, AstExpr* expr) : type(type), expr(expr) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstType* type;
	AstExpr* expr;
};

class AstPrefixExpr : public AstExpr {
public:
	enum Prefix {
		PLUS = 7,
		PPLUS,
		MINUS,
		MMINUS,
		DEREF,
		NOT = 23,
		ADDR = 21,
		NEGATE = 37
	};

	AstPrefixExpr(Prefix prefix, AstExpr* expr) : op(prefix), expr(expr) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	Prefix op;
	AstExpr* expr;
};

class AstPostfixExpr : public AstExpr {
public:
	enum Postfix {
		PPLUS = 8,
		MMINUS = 10,
		ACCESS = 38,
		PTRACCESS = 54,
		ARRAYACCESS = 100
	};

	AstPostfixExpr(Postfix postfix, AstExpr* expr) : expr(expr), op(postfix), type(0) {}
	AstPostfixExpr(Postfix postfix, AstExpr* expr, std::string name) : expr(expr), op(postfix), name(name), type(1) {}
	AstPostfixExpr(Postfix postfix, AstExpr* expr, AstExpr* index) : expr(expr), op(postfix), index(index), type(2) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	int type = 0;
	AstExpr* expr = nullptr;
	AstExpr* index = nullptr;
	Postfix op;
	std::string name = "";
};

class AstBinaryExpr : public AstExpr {
public:
	enum Binary {
		PLUS = 7,
		MINUS = 9,
		MUL = 11,
		DIV,
		MOD,
		EQU = 15,
		NEQ,
		LESS,
		LESS_EQU,
		GREATER,
		GREATER_EQU,
		AND,
		OR,
		XOR = 24,
		ANDAND,
		OROR
	};

	AstBinaryExpr(Binary op, AstExpr* left, AstExpr* right) : left(left), op(op), right(right) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstExpr* left;
	Binary op;
	AstExpr* right;
};

/* ----- STMTS ----- */

class AstStmt : public Ast {
public:
	virtual std::string toString() const override;
	virtual std::string prettyToString() const override;
};

class AstExprStmt : public AstStmt {
public:
	AstExprStmt(AstExpr* expr) : expr(expr) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstExpr* expr;
};

class AstAssignStmt : public AstStmt {
public:
	enum Assign {
		EQU = 14,
		PLUS = 58,
		MINUS,
		MUL,
		DIV,
		MOD,
	};

	AstAssignStmt(AstExpr* left, AstExpr* right, Assign op = Assign::EQU) : left(left), right(right), op(op) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstExpr* left;
	AstExpr* right;
	Assign op;
};

class AstCompStmt : public AstStmt {
public:
	AstCompStmt(std::vector<AstStmt*> stmts) : stmts(stmts) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::vector<AstStmt*> stmts;
};

class AstIfStmt : public AstStmt {
public:
	AstIfStmt(AstExpr* cond, AstStmt* stmt, AstStmt* elseStmt = nullptr) : cond(cond), stmt(stmt), elseStmt(elseStmt) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstExpr* cond;
	AstStmt* stmt;
	AstStmt* elseStmt;
};

class AstWhileStmt : public AstStmt {
public:
	AstWhileStmt(AstExpr* cond, AstStmt* stmt) : cond(cond), stmt(stmt) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstExpr* cond;
	AstStmt* stmt;
};

class AstReturnStmt : public AstStmt {
public:
	AstReturnStmt(AstExpr* expr, AstFunDecl* parentFunc) : expr(expr), funDecl(parentFunc) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstExpr* expr;
	AstFunDecl* funDecl;
	AstType* ofType = nullptr;
};

class AstVarStmt : public AstStmt {
public:
	AstVarStmt(AstVarDecl decl) : decl(decl) {}

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstVarDecl decl;
};
