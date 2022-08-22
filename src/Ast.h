#pragma once
#include <string>

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

private:
	// AstType type;
	// std::string name;
	// AstExpr expr;
};

class AstFunDecl : public AstDecl {

};

class AstParDecl : public AstDecl {

};

class AstTypeDecl : public AstDecl {

};

class AstStructDecl : public AstDecl {

};

/* ----- TYPES ----- */

class AstType : public Ast {
public:
	
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

	AstAtomType(Type type) : type(type) {}

	std::string toString() const override {
		std::string ret = "AtomType[";
		switch(type) {
			case INT:
				ret += "int]";
				break;
			case CHAR:
				ret += "char]";
				break;
			case BOOL:
				ret += "bool]";
				break;
			case VOID:
				ret += "void]";
				break;
			case FLOAT:
				ret += "float]";
				break;
			case NIL:
				ret += "nil]";
				break;
		}
	}
private:
	Type type;
};

class AstNamedType : public AstType {
public:
	AstNamedType(std::string name) : name(name) {}

	std::string toString() const override {
		return "NamedType[" + name + "]";
	}
private:
	std::string name;
};

class AstPtrType : public AstType {
public:
	AstPtrType(AstType* type, int depth) : type(type), depth(depth) {}

	std::string toString() const override {
		return "PtrType[" + type->toString() + "," + std::to_string(depth) + "]";
	}
private:
	AstType* type;
	int depth = 0;
};

class AstArrayType : public AstType {
public:
	AstArrayType(AstType* type, AstExpr* expr) : type(type), expr(expr) {}

	std::string toString() const override {
		return "ArrayType[" + type->toString() + "," + "]"; // expr->toString()
	}
private:
	AstType* type;
	AstExpr* expr;
};

/* ----- EXPRS ----- */

class AstExpr : public Ast {

};

class AstStmt : public Ast {

};