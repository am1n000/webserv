#include "../Includes/helpers.hpp"
#include <cctype>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <stdlib.h>

std::string helpers::to_string(int num)
{
  std::stringstream ss;
  ss <<  num;
  return ss.str();
}

std::string helpers::get_time()
{
	time_t now = time(0);

	tm* local_time = localtime(&now);

	int year = local_time->tm_year + 1900;
	int day = local_time->tm_mday;
	int hour = local_time->tm_hour;
	int min = local_time->tm_min;
	int sec = local_time->tm_sec;

	char month_name[4];
	char day_name[4];
	strftime(day_name, 4, "%a", local_time);
	strftime(month_name, 4, "%b", local_time);

	std::string now_time;
	now_time = day_name;
	now_time += ", ";
	now_time += helpers::to_string(day);
	now_time += " ";
	now_time += month_name;
	now_time += " ";
	now_time += helpers::to_string(year);
	now_time += " ";
	now_time += helpers::to_string(hour);
	now_time += ":";
	now_time += helpers::to_string(min);
	now_time += ":";
	now_time += helpers::to_string(sec);
	now_time += " ";
	now_time += "GMT";
	return (now_time);
}



std::string helpers::trim(const std::string str, char c) {
  std::string result;
  int start;
  int end;

  if(str.length() == 0)
    return "";
  start = 0;
  end = str.length();

  while(str[start] == c)
	start++;

  while (str[end - 1] == c || str[end - 1] == '\r')
	end--;

  result = str.substr(start, str.length() - (str.length() - end) - start);
  return result;
}

void helpers::recoverToken(char *token, char replacment) {
  int i = 0;

  while(token[i])
	i++;
  token[i] = replacment;
}

std::string helpers::getBracketsContent(char *buffer)  {
  int         openBrackets;
  int         size;
  int         start;
  std::string serverString;

  openBrackets = 1;
  size = 0;
  start = 0;
  while (std::isspace(buffer[start]))
	start++;
  if(buffer[start] != '{')
	throw std::exception();
  start++;
  while(openBrackets > 0) {
	if(buffer[start + size] == '{')
	  openBrackets++;
	else if (buffer[start + size] == '}')
	  openBrackets--;
	size++;
  }
  return std::string(buffer+start, buffer+start+size-1);
}

std::string helpers::getNextScop(char *buffer)  {
  int         openBrackets;
  int         size;
  int         start;
  std::string serverString;

  openBrackets = 1;
  size = 0;
  start = 0;
  while (buffer[start] != '{')
	start++;
  if(buffer[start] != '{')
	throw std::exception();
  start++;
  while(buffer [start + size] && openBrackets > 0) {
	if(buffer[start + size] == '{')
	  openBrackets++;
	else if (buffer[start + size] == '}')
	  openBrackets--;
	size++;
  }
  if(!buffer [start + size])
	throw std::runtime_error("unclosed scop");
  return std::string(buffer+start, buffer+start+size-1);
}

std::string helpers::unscope(const std::string& scope) {

  size_t start(0);
  size_t size(scope.length() - 1);

  while (std::isspace(scope[start]))
	start++;
  if(scope[start] != '{')
	throw std::runtime_error("invalid scope");
  start++;
  while(std::isspace(scope[size]))
	size--;
  if(scope[size] != '}')
	  throw std::runtime_error("invalid scope");
  return scope.substr(start, size - start);
}


std::string helpers::timeBasedName(std::string extenssion = "") {
  time_t t = time(0);
  struct tm* now = localtime(&t);

  char tmp[80];
  time_t x = clock();
  std::srand(x);
  strftime(tmp, 80, "%Y-%m-%d-%H%M%S", now);

  std::string ran(helpers::to_string(std::rand()));
  std::string name = ran + "_" + tmp + extenssion;
  return name;
}

void helpers::removeComments(std::string &buffer) {
  size_t pos;
  size_t end;
  while ((pos = buffer.find("#")) != std::string::npos) {
	  end = std::string::npos;
	  end = buffer.find("\n", pos);
	  if (end != std::string::npos) {
	  buffer.erase(pos, end - pos);
	  } else {
	  buffer.erase(pos, buffer.length() - 1);
	  }
  }
}
bool helpers::hasSpace(std::string& str) {
  for(size_t i(0); i < str.length(); i++) {
    if(std::isspace(str[i]))
      return true;
  }
  return false;
};

int helpers::stoi(const std::string &str) {
  std::istringstream ss(str);
  int result;
  ss >> std::hex >> result;
  return result;
}

// std::string helpers::to_string(int value) {
//   std::istringstream ss;
//   ss << value;
//   return ss.str();
// }

void  helpers::InternalServerError(int sock)
{
    std::string statusCodeResponse = "HTTP/1.1 500 Internal Server Error\r\nServer: webserv\r\n\r\n" ;
    statusCodeResponse += "<!DOCTYPE html>\n<html>\n<head><title>";
    statusCodeResponse += "505 Internal Server Error";
    statusCodeResponse += "</title></head>\n<body>\n<center><h1>";
    statusCodeResponse += "505 Internal Server Error";
    statusCodeResponse += "</h1></center>\n<hr><center>webserv</center>\n</body>\n</html>\n";
    std::cout << statusCodeResponse << std::endl;
    if (send(sock, statusCodeResponse.c_str(), statusCodeResponse.length(), 0) == -1)
      std::cerr << "error :send" << std::endl;
}

std::string handleRedirection(statusCodeExceptions &e, bool slash)
{
	std::string header = "HTTP/1.1 " + e.getValue();
	header +=  " Moved Permanently";
  header += "\r\nLocation: ";
  header += e.what();
  if (slash)
    header += "/";
	header += "\r\nServer: webserver-c\r\n\r\n";
  return (header);
}

void  helpers::displayStatusCodePage(statusCodeExceptions &e, int sock, bool slash, std::string& errorPage)
{
  std::string errorPagePath;
  if(errorPage.empty()) {
    errorPagePath = "./ErrorPages/" + e.getValue();
    errorPagePath +=	".html";
  }
  else 
    errorPagePath = errorPage; 
	std::ifstream	file(errorPagePath.c_str(), std::ios::ate);
	if (!file.is_open())
	{
    helpers::InternalServerError(sock);
		return;
	}
    int len = file.tellg();
    file.seekg(0, std::ios::beg);
	char buffer[len];
	file.read(buffer, len);
	buffer[len] = '\0';
	std::string header = "HTTP/1.1 " + e.getValue();
	header +=  " ";
	header += e.what();
	header += "\r\nServer: webserver-c\r\n\r\n";
	if (e.getValue() == "301")
    header = handleRedirection(e, slash);
	std::string a = buffer;	
	header += a;
	if (send(sock, header.c_str(), header.length(), 0) == -1)
      std::cerr << "error :send" << std::endl;
}


std::vector<std::string> helpers::splitPaths(std::string fullPath)
{
  std::vector<std::string> paths;
  size_t i = 1;
  while (i < fullPath.size())
  {
    std::string path;
    while (fullPath[i] != '/' && i < fullPath.size())
    {
      path += fullPath[i];
      i++;
    }
    paths.push_back(path);
    i++;
  }
  return (paths);
}

bool  helpers::withinRoot(std::vector<std::string> paths, std::vector<std::string> rootPaths)
{
  size_t size = rootPaths.size();
  for (size_t i = 0; i < paths.size(); i++)
  {
    if (paths[i] == "..")
      rootPaths.pop_back();
    else
      rootPaths.push_back(paths[i]);
    if (rootPaths.size() < size)
      return (1);
  }
  return (0);
}
