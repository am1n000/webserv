#include <dirent.h>
#include <iostream>
#include <sys/stat.h>


int list_directory(std::string path)
{
    struct stat fileStat;
    
    if (stat(path.c_str(), &fileStat) == 0)
    {
        if (S_ISDIR(fileStat.st_mode))
        {
            DIR* dirp = opendir(path.c_str());
            if (dirp == NULL)
            {
                std::cerr << "Error: failed to open directory" << std::endl;
                return 1;
            }
            struct dirent* dp;
            while ((dp = readdir(dirp)) != NULL)
            {
                if (dp->d_name[0] != '.') // Skip hidden files and directories
                {
                    struct stat filestat;
                    std::string filename = dp->d_name;
                    std::string fullpath = path + "/" + filename;
                    if (stat(fullpath.c_str(), &filestat) == 0) // Check if stat succeeded
                    {
                        std::time_t modified_time = filestat.st_mtime;
                        std::tm* modified_tm = std::localtime(&modified_time);
                        char modified_str[20];
                        std::strftime(modified_str, sizeof(modified_str), "%Y-%m-%d %H:%M:%S", modified_tm);
                        std::cout << filename << "      " << modified_str << "  .....    ";
                        std::cout << dp->d_ino <<  std::endl;
                    }
                }
            }
            closedir(dirp);
        }
        else
        {
            std::cout << path << " is not a directory." << std::endl;
        }
    }
    else
        std::cerr << "Path does not exist  " << path << std::endl;
    return (0);
}

int main()
{
    list_directory("./ressources");
}