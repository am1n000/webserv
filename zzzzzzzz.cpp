#include <iostream>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <string>
#include <fstream>

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
	strftime(month_name, 4, "%b", local_time);

	std::string now_time;
	now_time += my_tostring(day);
	now_time += ":";
	now_time += month_name;
	now_time += ":";
	now_time += my_tostring(year);
	now_time += ":";
	now_time += my_tostring(hour);
	now_time += ":";
	now_time += my_tostring(min);
	now_time += ":";
	now_time += my_tostring(sec);
	return (now_time);
}
int main ()
{
            std::fstream indexFile("ztest.html", std::ios::out);
            if (!indexFile.is_open())
                std::cout << " !!!!" << std::endl;
            indexFile << "<!DOCTYPE html>\n<html>\n<head>\n	<title>Hello World</title>\n</head>\n<body>\n	<h1>Hello World</h1>\n	<table style=\"text-align: left;\">\n		<tr>\n			<th>Number</th>\n			<th>Name</th>\n			<th>Date</th>\n		</tr>\n		<tr>\n			<td><a href=\"#\">123456123</a></td>\n			<td>John</td>\n			<td>May 9, 2023</td>\n		</tr>\n		<tr>\n			<td><a href=\"#\">456123456789</a></td>\n			<td>Janeadfadsfasdfasdfadsfasd</td>\n			<td>May 10, 2023</td>\n		</tr>\n		<tr>\n			<td><a href=\"#\">726544354348489</a></td>\n			<td>Bob</td>\n			<td>May 11, 2023</td>\n		</tr>\n	</table>\n</body>\n</html>";
        indexFile.close();
        indexFile.open("/Users/ael-rhai/Desktop/webserv/ztest.html", std::ios::in); // open file for reading
    // char a[2048];
    // a[1001] = '\0';
	// 	indexFile.read(a, 1000);
    //     std::cout << a << std::endl;
}