#include "Logger.h"
#include <iostream>
#include "Font.h"

void Logger::debug(const std::string& msg) {
	if(!debugEnabled) return;

	std::cout << msg << std::endl;
}

void Logger::log(const std::string& msg) {
	formatted(msg, false, nullptr);
	// std::cout << Font::italic << msg << Font::reset << std::endl;
}

void Logger::error(const std::string& msg) {
	std::cout << Font::fred << msg << Font::reset << std::endl;
}

void Logger::replaceAll(std::string& str, std::string find, std::string replace) {
	for(size_t index = 0; true; index += replace.size()) {
		index = str.find(find, index);
		if (index == std::string::npos) 
			break;

		str.replace(index, find.size(), replace);
	}
}
