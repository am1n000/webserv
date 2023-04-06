#include <dirent.h>
#include <iostream>
#include <sys/stat.h>


int list_directory(std::string path)
{
    std::string path = "ressources";
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
            while ((dp = readdir(dirp)) != NULL) {
                std::cout << dp->d_name << std::endl;
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