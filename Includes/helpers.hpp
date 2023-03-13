#pragma once

#include <string>

namespace helpers {
std::string trim(const std::string str, char c);
std::string getBracketsContent(char *buffer);
void recoverToken(char *token, char replacment);
};
