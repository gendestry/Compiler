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

public:
	Location loc;
};

/* ----- DECLS ----- */

class AstDecl : public Ast {
public:
public:
	virtual std::string toString() const override;
	virtual std::string prettyToString() const override;

	virtual AstType* getType() {}
};

class AstVarDecl : public AstDecl {
public:
	AstVarDecl(Location location, std::string name, AstType* type, AstExpr* expr = nullptr) : name(name), type(type), expr(expr) { loc = location;	}

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
	AstParDecl(Location location, std::vector<AstVarDecl>params) : params(params) {	loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
	int size() const { return params.size(); }
public:
	std::vector<AstVarDecl> params;
};

class AstFunDecl : public AstDecl {
public:
	AstFunDecl(Location location, std::string name, AstType* type, AstParDecl* params = nullptr, AstStmt* body = nullptr) 
		: name(name), type(type), params(params), body(body) { loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::string name;
	AstType* type;
	AstParDecl* params;
	AstStmt* body;
	bool hasReturn = false;
};

class AstTypeDecl : public AstDecl {
public:
	AstTypeDecl(Location location, std::string name, AstType* type) : name(name), type(type) { loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstType* type = nullptr;
	std::string name;
};

class AstStructDecl : public AstDecl {
public:
	AstStructDecl(Location location, std::string name, std::vector<AstVarDecl> fields) : name(name), fields(fields) { loc = location; }

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
	
	Type type;
public:
	virtual std::string toString() const override;
	virtual std::string prettyToString() const override;
	std::string getTypeName() const;
	std::string prettyGetTypeName() const;
};

class AstAtomType : public AstType {
public:
	AstAtomType(Location location, Type atomType) { type = atomType; loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
};

class AstNamedType : public AstType {
public:
	AstNamedType(Location location, std::string name) : name(name) { type = NAMED; loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::string name;
	AstDecl* declaration = nullptr;
};

class AstPtrType : public AstType {
public:
	AstPtrType(Location location, AstType* ptrType) : ptrType(ptrType) { type = PTR; loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstType* ptrType;
};

class AstArrayType : public AstType {
public:
	AstArrayType(Location location, AstType* arrayType, AstExpr* expr) : arrayType(arrayType), expr(expr) { type = ARRAY; loc = location; }

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
	bool isLValue = false;
};

class AstConstExpr : public AstExpr {
public:
	AstConstExpr(Location location, int value) : ivalue(value) { type = Token::NUMBER; loc = location; }
	AstConstExpr(Location location, char value) : cvalue(value) { type = Token::CHARACTER; loc = location; }
	AstConstExpr(Location location, bool value) : bvalue(value) { type = (value ? Token::TRUE : Token::FALSE); loc = location; }
	AstConstExpr(Location location, float value) : fvalue(value) { type = Token::FNUMBER; loc = location; }
	AstConstExpr(Location location, std::string value) : str(value) { type = Token::STRING; loc = location; }

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
	AstNamedExpr(Location location, std::string name) : name(name) { loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::string name;
	AstDecl* declaration = nullptr;
};

class AstCallExpr : public AstExpr {
public:
	AstCallExpr(Location location, std::string name, std::vector<AstExpr*> args) : name(name), args(args) { loc = location; }

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
	AstCastExpr(Location location, AstType* type, AstExpr* expr) : type(type), expr(expr) { loc = location; }

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

	AstPrefixExpr(Location location, Prefix prefix, AstExpr* expr) : op(prefix), expr(expr) { loc = location; }

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

	AstPostfixExpr(Location location, Postfix postfix, AstExpr* expr) : expr(expr), op(postfix), type(0) { loc = location; }
	AstPostfixExpr(Location location, Postfix postfix, AstExpr* expr, std::string name) : expr(expr), op(postfix), name(name), type(1) { loc = location; }
	AstPostfixExpr(Location location, Postfix postfix, AstExpr* expr, AstExpr* index) : expr(expr), op(postfix), index(index), type(2) { loc = location; }

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

	AstBinaryExpr(Location location, Binary op, AstExpr* left, AstExpr* right) : left(left), op(op), right(right) { loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstExpr* left;
	Binary op;
	AstExpr* right;
};

// class AstNewExpr : public AstExpr {
// public:
// 	AstNewExpr(Location location, AstType* type) : type(type) { loc = location; }

// 	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

// 	std::string toString() const override;
// 	std::string prettyToString() const override;
// public:
// 	AstType* type;
// };

// class AstDeleteExpr : public AstExpr {
// public:
// 	AstDeleteExpr(Location location, AstNamedExpr* expr) : expr(expr) { loc = location; }

// 	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

// 	std::string toString() const override;
// 	std::string prettyToString() const override;
// public:
// 	AstNamedExpr* expr;
// };


/* ----- STMTS ----- */

class AstStmt : public Ast {
public:
	virtual std::string toString() const override;
	virtual std::string prettyToString() const override;
};

class AstExprStmt : public AstStmt {
public:
	AstExprStmt(Location location, AstExpr* expr) : expr(expr) { loc = location; }

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

	AstAssignStmt(Location location, AstExpr* left, AstExpr* right, Assign op = Assign::EQU) : left(left), right(right), op(op) { loc = location; }

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
	AstCompStmt(Location location, std::vector<AstStmt*> stmts) : stmts(stmts) { loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	std::vector<AstStmt*> stmts;
};

class AstIfStmt : public AstStmt {
public:
	AstIfStmt(Location location, AstExpr* cond, AstStmt* stmt, AstStmt* elseStmt = nullptr) : cond(cond), stmt(stmt), elseStmt(elseStmt) { loc = location; }

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
	AstWhileStmt(Location location, AstExpr* cond, AstStmt* stmt) : cond(cond), stmt(stmt) { loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstExpr* cond;
	AstStmt* stmt;
};

class AstReturnStmt : public AstStmt {
public:
	AstReturnStmt(Location location, AstExpr* expr, AstFunDecl* parentFunc) : expr(expr), funDecl(parentFunc) { loc = location; }

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
	AstVarStmt(Location location, AstVarDecl decl) : decl(decl) { loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;
public:
	AstVarDecl decl;
};

class AstFunStmt : public AstStmt {
public:
	AstFunStmt(Location location, AstFunDecl* decl) : decl(decl) { loc = location; }

	bool accept(Visitor* visitor, Phase phase) override { return visitor->visit(this, phase); }

	std::string toString() const override;
	std::string prettyToString() const override;

	~AstFunStmt() { delete decl; }
public:
	AstFunDecl* decl;
};
