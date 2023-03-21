#include "../Includes/Utils.hpp"

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