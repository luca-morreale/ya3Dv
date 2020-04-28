#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "utils.hpp"

using namespace boost::filesystem;

std::vector<std::string> read_folder_list(std::string folder_list_file);

std::vector<std::string> list_files(std::string folder);


#endif // FILESYSTEM_HPP_
