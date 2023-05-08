#pragma once

#include <string>

namespace helpers {
std::string trim(const std::string str, char c = ' ');
std::string getBracketsContent(char *buffer);
void recoverToken(char *token, char replacment);
std::string getNextScop(char *buffer);
std::string unscope(const std::string& scope);
std::string timeBasedName(std::string extenssion);
void        removeComments(std::string& buffer);
};
