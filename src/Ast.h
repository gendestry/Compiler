#pragma once
#include <string>
#include <vector>
#include "Token.h"
#include <iostream>

class AstDecl;
class AstType;
class AstExpr;
class AstStmt;

class Ast {
public:
	virtual std::string toString() const = 0;
};

/* ----- DECLS ----- */

class AstDecl : public Ast {
private:
	// std::string name;
	// AstType* type;
public:
	// AstDecl(std::string name, AstType* type) : name(name), type(type) {}
};

class AstVarDecl : public AstDecl {
public:
	AstVarDecl(std::string name, AstType* type, AstExpr* expr = nullptr) : name(name), type(type), expr(expr) {}

	std::string toString() const override;
private:
	AstType* type = nullptr;
	AstExpr* expr = nullptr;
	std::string name;
};

class AstParDecl : public AstDecl {
public:
	AstParDecl(std::vector<AstVarDecl>params) : params(params) {}

	std::string toString() const override;
private:
	std::vector<AstVarDecl> params;
};

class AstFunDecl : public AstDecl {
public:
	AstFunDecl(std::string name, AstType* type, AstParDecl* params = nullptr, AstStmt* body = nullptr) : name(name), type(type), params(params), body(body) {}

	std::string toString() const override;
private:
	std::string name;
	AstType* type;
	AstParDecl* params;
	AstStmt* body;
};

class AstTypeDecl : public AstDecl {
public:
	AstTypeDecl(std::string name, AstType* type) : name(name), type(type) {}

	std::string toString() const override;
private:
	AstType* type = nullptr;
	std::string name;
};

class AstStructDecl : public AstDecl {
public:
	AstStructDecl(std::string name, std::vector<AstVarDecl> fields) : name(name), fields(fields) {}

	std::string toString() const override;
private:
	std::string name;
	std::vector<AstVarDecl> fields;
};

/* ----- TYPES ----- */

class AstType : public Ast {
public:
	virtual std::string toString() const override;
};

class AstAtomType : public AstType {
public:
	enum Type {
		INT = 1,
		CHAR,
		BOOL,
		VOID,
		FLOAT,
		NIL
	};

	AstAtomType(Type type) : type(type) {  }

	std::string toString() const override;
private:
	Type type;
};

class AstNamedType : public AstType {
public:
	AstNamedType(std::string name) : name(name) {}

	std::string toString() const override;
private:
	std::string name;
};

class AstPtrType : public AstType {
public:
	AstPtrType(AstType* type, int depth) : type(type), depth(depth) {}

	std::string toString() const override;
private:
	AstType* type;
	int depth = 0;
};

class AstArrayType : public AstType {
public:
	AstArrayType(AstType* type, AstExpr* expr) : type(type), expr(expr) {}

	std::string toString() const override;
private:
	AstType* type;
	AstExpr* expr;
};

/* ----- EXPRS ----- */

class AstExpr : public Ast {
public:
	virtual std::string toString() const override;
};

class AstConstExpr : public AstExpr {
public:
	AstConstExpr(int value) : ivalue(value) { type = Token::NUMBER; }
	AstConstExpr(char value) : cvalue(value) { type = Token::CHARACTER; }
	AstConstExpr(bool value) : bvalue(value) { type = (value ? Token::TRUE : Token::FALSE); }
	AstConstExpr(float value) : fvalue(value) { type = Token::FNUMBER; }
	AstConstExpr(std::string value) : str(value) { type = Token::STRING;}

	std::string toString() const override;
private:
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

	std::string toString() const override;
private:
	std::string name;
};

class AstCallExpr : public AstExpr {
public:
	AstCallExpr(std::string name, std::vector<AstExpr*> args) : name(name), args(args) {}

	std::string toString() const override;
private:
	std::string name;
	std::vector<AstExpr*> args;
};

class AstCastExpr : public AstExpr {
public:
	AstCastExpr(AstType* type, AstExpr* expr) : type(type), expr(expr) {}

	std::string toString() const override;
private:
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

	AstPrefixExpr(Prefix prefix, AstExpr* expr) : op(prefix), expr(expr) {
		
	}

	std::string toString() const override;
private:
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

	std::string toString() const override;
private:
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

	std::string toString() const override;
private:
	AstExpr* left;
	Binary op;
	AstExpr* right;
};

/* ----- STMTS ----- */

class AstStmt : public Ast {
public:
	virtual std::string toString() const override;
};

class AstExprStmt : public AstStmt {
public:
	AstExprStmt(AstExpr* expr) : expr(expr) {}

	std::string toString() const override;
private:
	AstExpr* expr;
};

class AstAssignStmt : public AstStmt {
public:
	enum Assign {
		EQU = 15,
		PLUS = 58,
		MINUS,
		MUL,
		DIV,
		MOD,
	};

	AstAssignStmt(AstExpr* left, AstExpr* right, Assign op = Assign::EQU) : left(left), right(right), op(op) {}

	std::string toString() const override;
private:
	AstExpr* left;
	AstExpr* right;
	Assign op;
};

class AstCompStmt : public AstStmt {
public:
	AstCompStmt(std::vector<AstStmt*> stmts) : stmts(stmts) {}

	std::string toString() const override;
private:
	std::vector<AstStmt*> stmts;
};

class AstIfStmt : public AstStmt {
public:
	AstIfStmt(AstExpr* cond, AstStmt* stmt, AstStmt* elseStmt = nullptr) : cond(cond), stmt(stmt), elseStmt(elseStmt) {}

	std::string toString() const override;
private:
	AstExpr* cond;
	AstStmt* stmt;
	AstStmt* elseStmt;
};

class AstWhileStmt : public AstStmt {
public:
	AstWhileStmt(AstExpr* cond, AstStmt* stmt) : cond(cond), stmt(stmt) {}

	std::string toString() const override;
private:
	AstExpr* cond;
	AstStmt* stmt;
};

class AstReturnStmt : public AstStmt {
public:
	AstReturnStmt(AstExpr* expr) : expr(expr) {}

	std::string toString() const override;
private:
	AstExpr* expr;
};

class AstVarStmt : public AstStmt {
public:
	AstVarStmt(AstVarDecl decl) : decl(decl) {}

	std::string toString() const override;
private:
	AstVarDecl decl;
};
