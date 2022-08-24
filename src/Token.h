#pragma once
#include <vector>
#include <string>
#include <iostream>

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

	Token(TokenType type, int line, int start, int len = 0, std::string text = "") : type(type), line(line + 1), start(start), len(len), text(text) {}

	TokenType getType() const { return type; }
	int getLine() const { return line; }
	int getStart() const { return start; }
	int getLen() const { return len; }
	std::string getText() const { return text; }

	std::string getName() const { return tokenNames[(int)type]; }

	friend std::ostream& operator<<(std::ostream& os, const Token& token) {
		os << token.getName() << "(" << token.start << "," << token.start + token.len <<  ")";
		return os;
	}
private:
	TokenType type;
	int line;
	int start;
	int len;
	std::string text;
};