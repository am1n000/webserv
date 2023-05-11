#include "../Includes/Utils.hpp"


std::string uniqueFilename()
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
	return (now_time);
}


bool checkIndex(std::string filename, std::vector<std::string> indexes)
{
    for (size_t i = 0; i < indexes.size(); i++)
    {
        if (indexes[i] == filename)
            return (1);
    }
    return (0);
}


std::pair<int, std::string> directoryCheck(std::string path, int autoindex, std::vector<std::string> indexes)
{
    struct stat fileStat;
    std::pair<int, std::string> result;
    result.first = 0;
    result.second = path;
                std::cout << "here" << std::endl;
    if (stat(path.c_str(), &fileStat) == 0)
    {
        if (S_ISDIR(fileStat.st_mode))
        {
            DIR* dirp = opendir(path.c_str());
            if (dirp == NULL)
                throw (Forbidden());
            if (path[path.size() - 1] != '/')
            {
                std::cout << "but not here" << std::endl;
                throw (MovedPermanently());
            }
            struct dirent* dp;
            std::string indexFilename = path + "/.";
            indexFilename += uniqueFilename() + ".html";
            std::fstream indexFile(indexFilename.c_str(), std::ios::out);
            indexFile << "<!DOCTYPE html>\n<html>\n<head>\n	<title>Hello World</title>\n</head>\n<body>\n	<h1>Hello World</h1>\n	<table style=\"text-align: left;\">\n		<tr>\n			<th>Name</th>\n			<th>Last modified</th>\n			<th>Size</th>\n		</tr>\n";
            while ((dp = readdir(dirp)) != NULL)
            {
                if (dp->d_name[0] != '.') // Skip hidden files and directories
                {
                    std::string filename = dp->d_name;
                    if (checkIndex(filename, indexes))
                    {
                        indexFile.close();
                        std::remove(indexFilename.c_str());
                        result.first = 1;
                        result.second = path + "/" + filename;
                        return (result);
                    }
                    std::string fullpath = path + "/" + filename;
                    if (stat(fullpath.c_str(), &fileStat) == 0) // Check if stat succeeded
                    {
                        std::time_t modified_time = fileStat.st_mtime;
                        std::tm* modified_tm = std::localtime(&modified_time);
                        char modified_str[20];
                        std::strftime(modified_str, sizeof(modified_str), "%Y-%m-%d %H:%M:%S", modified_tm);
                        indexFile << "		<tr>\n			<td><a href=\"" + path;
                        indexFile << "/" + filename; 
                        indexFile << "\">";
                        indexFile << filename + "</a></td>\n";
                        indexFile << "			<td>";
                        indexFile << modified_str;
                        indexFile << "</td>\n";
                        indexFile << "			<td>" << my_tostring(fileStat.st_size) << "</td>\n		</tr>\n";
                    }
                }
            }
            indexFile << "	</table>\n</body>\n</html>";
            closedir(dirp);
            indexFile.close();
            if (!autoindex)
            {
                std::remove(indexFilename.c_str());
                throw (Forbidden());
            }
            result.first = 2;
            result.second = indexFilename;
            return (result);
        }
    }
    return (result);
}

// int main()
// {
//     directoryCheck("./ressources");
// }