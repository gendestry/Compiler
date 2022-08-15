#include "Lexan.h"
#include <fstream>
#include <ctype.h>

Lexan::Lexan() {}

bool Lexan::parse(const std::string& file) {
	tokens.clear();
	std::ifstream input(file);
	
	if (!input.is_open()) {
		std::cout << "Could not open file: " << file << std::endl;
		return false;
	}

	std::string line;
	for(int i = 0; std::getline(input, line); i++) {
		if(!parseLine(line, i)) {
			return false;
		}
	}

	input.close();

	return true;
}

bool Lexan::parseLine(std::string line, int i) {
	int start = 0;
	bool done = false;

	auto checkKeyword = [&line](int& j, const std::string& keyword) {
		if(j + keyword.size() - 1 <= line.size() && line.substr(j, keyword.size()) == keyword) {
			return true;
		}
		return false;
	};

	for(int j = 0; j < line.size() && !done; j++) {
		char c = line[j];
		char previous = j > 0 ? line[j - 1] : ' ';
		switch(c) {
			case '"':
				start = j++;
				for(; j < line.size(); j++) {
					c = line[j];

					if(c == '"' && previous != '\\') {
						tokens.emplace_back(Token::STRING, i, start, j - start);
						break;
					}
					previous = c;
				}

				break;

			case '\t':
			case ' ' : break;

			case '+' : 
				if (j + 1 < line.size() && line[j + 1] == '+') {
					tokens.emplace_back(Token::PPLUS, i, j++, 1);
				} else {
					tokens.emplace_back(Token::PLUS, i, j);
				}
				break;
			case '-' :
				if (j + 1 < line.size() && line[j + 1] == '-') {
					tokens.emplace_back(Token::MMINUS, i, j++, 1);
				} else {
					tokens.emplace_back(Token::MINUS, i, j);
				}
				break;
			case '*' : tokens.emplace_back(Token::MULTIPLY, i, j); break;
			case '%' : tokens.emplace_back(Token::MODULO, i, j); break;
			case '/' : 
				if (j + 1 < line.size() && line[j + 1] == '/') {
					done = true;
					break;
				}
				tokens.emplace_back(Token::DIVIDE, i, j); break;
				break;
			
			case '=':
				if (j + 1 < line.size() && line[j + 1] == '=') {
					tokens.emplace_back(Token::EQUAL, i, j++, 1);
				} else {
					tokens.emplace_back(Token::ASSIGN, i, j);
				}
				break;

			case '<':
				if (j + 1 < line.size() && line[j + 1] == '=') {
					tokens.emplace_back(Token::LESS_THAN_EQUAL, i, j++, 1);
				} else {
					tokens.emplace_back(Token::LESS_THAN, i, j);
				}
				break;

			case '>':
				if (j + 1 < line.size() && line[j + 1] == '=') {
					tokens.emplace_back(Token::GREATER_THAN_EQUAL, i, j++, 1);
				} else {
					tokens.emplace_back(Token::GREATER_THAN, i, j);
				}
				break;

			case '!' : 
				if (j + 1 < line.size() && line[j + 1] == '=') {
					tokens.emplace_back(Token::NOT_EQUAL, i, j++, 1);
				} else {
					tokens.emplace_back(Token::NOT, i, j);
				}
				break;

			case '&' : tokens.emplace_back(Token::AND, i, j); break;
			case '|' : tokens.emplace_back(Token::OR, i, j); break;
			case '^' : tokens.emplace_back(Token::XOR, i, j); break;

			case '[' : tokens.emplace_back(Token::LBRACKET, i, j); break;
			case ']' : tokens.emplace_back(Token::RBRACKET, i, j); break;
			case '{' : tokens.emplace_back(Token::LBRACE, i, j); break;
			case '}' : tokens.emplace_back(Token::RBRACE, i, j); break;
			case '(' : tokens.emplace_back(Token::LPAREN, i, j); break;
			case ')' : tokens.emplace_back(Token::RPAREN, i, j); break;

			case ',' : tokens.emplace_back(Token::COMMA, i, j); break;
			case ';' : tokens.emplace_back(Token::SEMICOLON, i, j); break;
			case ':' : tokens.emplace_back(Token::COLON, i, j); break;
			case '?' : tokens.emplace_back(Token::QUESTION, i, j); break;
			case '~' : tokens.emplace_back(Token::ELLIPSIS, i, j); break;
			case '.' : tokens.emplace_back(Token::DOT, i, j); break;

			default:
				start = j;
				// number
				if (isdigit(c)) {
					for(; j < line.size() && isdigit(line[j]); j++);
					if(isalpha(line[j])) {
						std::cout << "Lexan: Invalid number on line " << (i+1) << "[" << start << "]" << std::endl;
						return false;
					}
					j--;
					tokens.emplace_back(Token::NUMBER, i, start, j - start);
				}
				// keywords and ids
				else if (isalpha(c)) {
					// check for int void char bool
					if (checkKeyword(j, "int")) {
						tokens.emplace_back(Token::INT, i, j, 2); j+=2;
					} else if (checkKeyword(j, "void")) {
						tokens.emplace_back(Token::VOID, i, j, 3); j+=3;
					} else if (checkKeyword(j, "char")) {
						tokens.emplace_back(Token::CHAR, i, j, 3); j+=3;
					} else if (checkKeyword(j, "bool")) {
						tokens.emplace_back(Token::BOOL, i, j, 3); j+=3;
					} else if (checkKeyword(j, "null")) {
						tokens.emplace_back(Token::NIL, i, j, 3); j+=3;
					} else if (checkKeyword(j, "true")) {
						tokens.emplace_back(Token::TRUE, i, j, 3); j+=3;
					} else if (checkKeyword(j, "false")) {
						tokens.emplace_back(Token::FALSE, i, j, 4); j+=4;
		
					} else if (checkKeyword(j, "if")) {
						tokens.emplace_back(Token::IF, i, j, 1); j+=1;
					} else if (checkKeyword(j, "else")) {
						tokens.emplace_back(Token::ELSE, i, j, 3); j+=3;
					} else if (checkKeyword(j, "while")) {
						tokens.emplace_back(Token::WHILE, i, j, 4); j+=4;
					} else if (checkKeyword(j, "for")) {
						tokens.emplace_back(Token::FOR, i, j, 2); j+=2;
					} else if (checkKeyword(j, "break")) {
						tokens.emplace_back(Token::BREAK, i, j, 4); j+=4;
					} else {
						for(; j < line.size() && isalpha(line[j]); j++);
						j--;
						tokens.emplace_back(Token::IDENTIFIER, i, start, j - start);
					}
				}
				else {
					std::cout << "Lexan: Invalid character on line " << (i+1) << "[" << j << "]" << std::endl;
					return false;
				}
				break;
		};
	}

	return true;
}

void Lexan::printTokens() {
	int l = 0;
	for(auto& t : tokens) {
		if(l != t.getLine())
			std::cout << std::endl;
		std::cout << t << " ";
		l = t.getLine();
	}
	std::cout << std::endl;
}