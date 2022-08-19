#include "Font.h"
#include <sstream>

const std::string Font::reset = "\x1B[0m";
const std::string Font::bold = "\x1B[1m";
const std::string Font::faint = "\x1B[2m";
const std::string Font::italic = "\x1B[3m";
const std::string Font::underline = "\x1B[4m";
const std::string Font::inverse = "\x1B[5m";
const std::string Font::striked = "\x1B[6m";

const std::string Font::fblack = "\x1B[30m";
const std::string Font::fred = "\x1B[31m";
const std::string Font::fgreen = "\x1B[32m";
const std::string Font::fyellow = "\x1B[33m";
const std::string Font::fblue = "\x1B[34m";
const std::string Font::fmagenta = "\x1B[35m";
const std::string Font::fcyan = "\x1B[36m";
const std::string Font::fwhite = "\x1B[37m";

const std::string Font::bblack = "\x1B[40m";
const std::string Font::bred = "\x1B[41m";
const std::string Font::bgreen = "\x1B[42m";
const std::string Font::byellow = "\x1B[43m";
const std::string Font::bblue = "\x1B[44m";
const std::string Font::bmagenta = "\x1B[45m";
const std::string Font::bcyan = "\x1B[46m";
const std::string Font::bwhite = "\x1B[47m";

std::string Font::byColorCode(int r, int g, int b, bool fg) {
    std::stringstream ss;
    ss << "\x1B[" << (fg ? 3 : 4) << "8;2;" << r << ";" << g << ";"<< b << "m";
    return ss.str();
} 