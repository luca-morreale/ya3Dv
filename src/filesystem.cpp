
#include "filesystem.hpp"

std::vector<std::string> read_folder_list(std::string folder_list_file)
{
    std::vector<std::string> folders;

    std::ifstream cin(folder_list_file);

    if (cin.is_open())
    {
        std::string path;
        while (cin >> path)
        {
            trim(path);
            if (path.back() == '/') {
                path = path.substr(0, path.size()-1);
            }
            folders.push_back(path);
        }
    }
    cin.close();

    return folders;
}


std::vector<std::string> list_files(std::string folder)
{
    std::vector<std::string> files_list;

    path p(folder);
    directory_iterator end_itr;

    // cycle through the directory
    for (directory_iterator itr(p); itr != end_itr; ++itr)
    {
        if (is_regular_file(itr->path()))
        {
            files_list.push_back(itr->path().string());
        }
    }

    std::sort(files_list.begin(), files_list.end());
    return files_list;
}
