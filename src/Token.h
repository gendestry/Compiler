#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Location.h"

class Token {
public:
	static std::vector<std::string> tokenNames;
public:
	enum TokenType {
		IDENTIFIER,
		INT,
		CHAR,
		BOOL,
		VOID,
		FLOAT,
		NIL,

		PLUS,
		PPLUS,
		MINUS,
		MMINUS,
		MULTIPLY,
		DIVIDE,
		MODULO,
		ASSIGN,

		EQUAL,
		NOT_EQUAL,
		LESS_THAN,
		LESS_THAN_EQUAL,
		GREATER_THAN,
		GREATER_THAN_EQUAL,

		AND,
		OR,
		NOT,
		XOR,

		ANDAND,
		OROR,

		LPAREN,
		RPAREN,
		LBRACE,
		RBRACE,
		LBRACKET,
		RBRACKET,

		COMMA,
		SEMICOLON,
		COLON,
		QUESTION,
		ELLIPSIS,
		DOT,

		NUMBER,
		FNUMBER,
		CHARACTER,
		STRING,
		TRUE,
		FALSE,
		IF,
		ELSE,
		WHILE,
		FOR,
		BREAK,
		STRUCT,
		SIZEOF,
		TYPEDEF,
		RETURN,
		PTR,

		NEW,
		DEL,
		CONTINUE,
		
		PLUSEQU,
		MINUSEQU,
		MULTEQU,
		DIVEQU,
		MODEQU,

		ERROR
	};

	Token(TokenType type, int line, int start, int len = 0, std::string text = "") : type(type), text(text) { 
		location.line = line + 1;
		location.start = start;
		location.end = start + len;
	}

	TokenType getType() const { return type; }
	int getLine() const { return location.line; }
	int getStart() const { return location.start; }
	int getEnd() const { return location.end; }
	std::string getText() const { return text; }

	std::string getName() const { return tokenNames[(int)type]; }

	friend std::ostream& operator<<(std::ostream& os, const Token& token) {
		os << token.getName() << "(" << token.location.start << "," << token.location.end <<  ")";
		return os;
	}
private:
	TokenType type;
	Location location;
	std::string text;
};
