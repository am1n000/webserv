#include "../Includes/helpers.hpp"
#include <cctype>
#include <exception>
#include <stdexcept>
#include <string>

char *my_tostring(int num)
{
	int i = 0, rem, len = 0, n;
 
	n = num;
	while (n != 0)
	{
		len++;
		n /= 10;
	}
	if  (len == 1)
		len = 2;
	char *str = new char[len];
	if  (len == 1)
	{
		str[0] = '0';
		i = 1;
	}
	for (;i < len; i++)
	{
		rem = num % 10;
		num = num / 10;
		str[len - (i + 1)] = rem + '0';
	}
	str[len] = '\0';
	return (str);
}

std::string get_time()
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
	now_time += my_tostring(day);
	now_time += " ";
	now_time += month_name;
	now_time += " ";
	now_time += my_tostring(year);
	now_time += " ";
	now_time += my_tostring(hour);
	now_time += ":";
	now_time += my_tostring(min);
	now_time += ":";
	now_time += my_tostring(sec);
	now_time += " ";
	now_time += "GMT";
	return (now_time);
}



std::string helpers::trim(const std::string str, char c) {
  std::string result;
  int start;
  int end;

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
  srand(x);
  strftime(tmp, 80, "%Y-%m-%d-%H%M%S", now);

  std::string name = std::to_string(rand()) + "_" + tmp + extenssion;
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

void  displayStatusCodePage(statusCodeExceptions &e, int sock, std::string ressourcePath)
{
	std::string statusCodePath = "/Users/ael-rhai/Desktop/webserv/HTTPStatusCodes/" + e.getValue();
	statusCodePath +=	".html";
	
	std::ifstream	file(statusCodePath.c_str(), std::ios::ate);
	if (!file.is_open())
	{
		std::cerr << "status code file " << e.getValue() << "  could not be opened!" << std::endl;
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
	if (e.getValue() == "301")
	{
		header += "\r\nLocation: ";
		header += ressourcePath + "/";
	}
	header += "\r\nServer: webserver-c\r\n\r\n";
	std::string a = buffer;	
	header += a;
	send(sock, header.c_str(), header.length(), 0);
}


std::vector<std::string> splitPaths(std::string fullPath)
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

bool  widdinRoot(std::vector<std::string> paths, std::vector<std::string> rootPaths)
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