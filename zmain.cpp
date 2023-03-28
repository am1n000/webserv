#include <dirent.h>
#include <iostream>

int main()
{
    DIR* dirp = opendir("ressources");
    if (dirp == NULL) {
        std::cerr << "Error: failed to open directory" << std::endl;
        return 1;
    }
    struct dirent* dp;
    while ((dp = readdir(dirp)) != NULL) {
        std::cout << dp->d_name << std::endl;
    }
    closedir(dirp);
    return 0;
}