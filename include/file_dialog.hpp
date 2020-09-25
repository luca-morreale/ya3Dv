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
namespace FileDialog {

    enum class SortOrder {
        Up,
        Down,
        None
    };

    static bool fileDialogOpen = false;

    bool IsFileDialogOpen();
    void OpenFileDialog(bool show);
    void ShowFileDialog(std::vector<std::string> &selection);

    std::string get_file_date(fs::directory_entry &entry);

    void set_sort_order_by_name(SortOrder &name,
                        SortOrder &date,
                        SortOrder &size,
                        SortOrder &type);
    void set_sort_order_by_date(SortOrder &name,
                        SortOrder &date,
                        SortOrder &size,
                        SortOrder &type);
    void set_sort_order_by_size(SortOrder &name,
                        SortOrder &date,
                        SortOrder &size,
                        SortOrder &type);
    void set_sort_order_by_type(SortOrder &name,
                        SortOrder &date,
                        SortOrder &size,
                        SortOrder &type);

    void sort_by_name(std::vector<fs::directory_entry> &list, 
                        SortOrder &order);
    void sort_by_date(std::vector<fs::directory_entry> &list, 
                        SortOrder &order);
    void sort_by_type(std::vector<fs::directory_entry> &list, 
                        SortOrder &order);
    void sort_by_size(std::vector<fs::directory_entry> &list, 
                        SortOrder &order);

} // namespace FileDialog
} // namespace UI

#endif // L2DFILEDIALOG_HPP_
