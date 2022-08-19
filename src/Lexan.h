#pragma once
#include <vector>
#include <string>
#include <iostream>

class Token {
public:
	const std::vector<std::string> tokenNames = {"ID", "INT", "CHAR", "BOOL", "VOID", "NIL", "PLUS", "PPLUS", "MINUS", "MMINUS", "MULTIPLY", "DIVIDE", "MODULO", "ASSIGN", "EQUAL", "NOT_EQUAL", "LESS_THAN", "LESS_THAN_EQUAL", "GREATER_THAN", "GREATER_THAN_EQUAL", "AND", "OR", "NOT", "XOR", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACKET", "RBRACKET", "COMMA", "SEMICOLON", "COLON", "QUESTION", "ELLIPSIS", "DOT", "NUMBER", "STRING", "TRUE", "FALSE", "IF", "ELSE", "WHILE", "FOR", "BREAK"};
public:
	enum TokenType {
		IDENTIFIER,
		INT,
		CHAR,
		BOOL,
		VOID,
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
		STRING,
		TRUE,
		FALSE,
		IF,
		ELSE,
		WHILE,
		FOR,
		BREAK
	};

	Token(TokenType type, int line, int start, int len = 0, std::string text = "") : type(type), line(line), start(start), len(len), text(text) {}

	TokenType getType() const { return type; }
	int getLine() const { return line; }
	int getStart() const { return start; }
	int getLen() const { return len; }
	std::string getText() const { return text; }

	friend std::ostream& operator<<(std::ostream& os, const Token& token) {
		os << token.tokenNames[token.type] << "(" << token.start << "," << token.start + token.len <<  ")";
		return os;
	}
private:
	TokenType type;
	int line;
	int start;
	int len;
	std::string text;
};


class Lexan {
	std::vector<Token> tokens;
public:
	Lexan();

	bool parse(const std::string& file);
	bool parseLine(std::string& line, int i);

	void printTokens();

	const std::vector<Token>& getTokens() const { return tokens; }
};