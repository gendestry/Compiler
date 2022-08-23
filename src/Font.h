#pragma once
#include <string>

class Font {
public:
    static const std::string reset, bold, faint, italic, underline, inverse, striked;
    static const std::string fblack, fred, fgreen, fyellow, fblue, fmagenta, fcyan, fwhite;
    static const std::string bblack, bred, bgreen, byellow, bblue, bmagenta, bcyan, bwhite;
public:
    static std::string byColorCode(int r, int g, int b, bool fg = true);
};
