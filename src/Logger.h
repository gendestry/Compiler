#pragma once
#include <string>
#include <stdio.h>
#include "Font.h"

class Logger {
private:
	bool debugEnabled = false;

	Logger() {}

	void replaceAll(std::string& str, std::string find, std::string replace);
public:
	static Logger& getInstance() {
		static Logger instance;
		return instance;
	}

	void debug(const std::string& msg);
	void log(const std::string& msg);
	void error(const std::string& msg);

	template<typename... Args>
	void debug(std::string format, Args... args) {
		format.insert(0, Font::italic);
		format.append(Font::reset);

		printf(format.c_str(), args...);
	}

	template<typename... Args>
	void log(std::string format, Args... args) {
		printf(format.c_str(), args...);
	}

	template<typename... Args>
	void error(std::string format, Args... args) {
		format.insert(0, Font::fred);
		format.append(Font::reset);

		printf(format.c_str(), args...);
	}

	template<typename... Args>
	void formatted(std::string format, Args... args) {
		replaceAll(format, "#i", Font::italic);
		replaceAll(format, "#b", Font::bold);
		replaceAll(format, "#u", Font::underline);
		replaceAll(format, "#r", Font::reset);

		printf(format.c_str(), args...);
	}
};