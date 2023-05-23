#pragma once

#include <string>
#include "Exceptions.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sys/socket.h>
#include <vector>

namespace helpers {
std::string trim(const std::string str, char c = ' ');
std::string getBracketsContent(char *buffer);
void recoverToken(char *token, char replacment);
std::string getNextScop(char *buffer);
std::string unscope(const std::string& scope);
std::string timeBasedName(std::string extenssion);
void        removeComments(std::string& buffer);
bool        hasSpace(std::string& str);
int         stoi(const std::string& str);
std::string to_string(int value);
};

char *my_tostring(int num);
std::string get_time();
void  displayStatusCodePage(statusCodeExceptions &e, int sock, std::string path);
std::vector<std::string> splitPaths(std::string fullPath);
bool  widdinRoot(std::vector<std::string> paths, std::vector<std::string> rootPaths);
