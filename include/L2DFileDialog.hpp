#ifndef L2DFILEDIALOG_HPP_
#define L2DFILEDIALOG_HPP_

#include <imgui.h>
#include <imgui_internal.h>
#include <chrono>
#include <string>
#include <experimental/filesystem>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>
#include <array>


using namespace std::chrono_literals;

namespace fs = std::experimental::filesystem;

namespace UI {

    enum class FileDialogType {
        OpenFile,
        SelectFolder
    };
    enum class FileDialogSortOrder {
        Up,
        Down,
        None
    };

    extern bool fileDialogOpen;

    void ShowFileDialog(std::vector<std::string> &selection);

} // namespace UI

#endif // L2DFILEDIALOG_HPP_
