
#include "file_dialog.hpp"

namespace UI {
namespace FileDialog {

    bool IsFileDialogOpen()
    {
        return fileDialogOpen;
    }

    void OpenFileDialog(bool show)
    {
        fileDialogOpen = show;
    }

    void ShowFileDialog(std::vector<std::string> &selection)
    {
        static std::array<bool, 1000> selected_files;
        static int fileDialogFolderSelectIndex = 0;
        static std::string currentPath = ".";
        static std::string currentFile = "";
        static std::string currentFolder = "";

        static SortOrder fileNameSortOrder = SortOrder::None;
        static SortOrder fileDateSortOrder = SortOrder::None; 
        static SortOrder fileSizeSortOrder = SortOrder::None;
        static SortOrder fileTypeSortOrder = SortOrder::None;
        static SortOrder folderNameSortOrder = SortOrder::None;
        static SortOrder folderDateSortOrder = SortOrder::None; 
        static SortOrder folderSizeSortOrder = SortOrder::None;
        static SortOrder folderTypeSortOrder = SortOrder::None;
         
        std::vector<fs::directory_entry> files;
        std::vector<fs::directory_entry> folders;

        // converts initial relative path to absolute
        currentPath = fs::canonical(fs::path(currentPath)).string();


        // ImGui::SetNextWindowSize(ImVec2(740.0f, 395.0f));
        ImGui::Begin("Select a file", nullptr);

        // List all files in current path
        try {
            for (auto& p : fs::directory_iterator(currentPath)) {
                if (fs::is_directory(p)) {
                    // remove ghost folders (hidden)
                    if (p.path().stem().string().empty()) continue;
                    folders.push_back(p);
                }
                else {
                    files.push_back(p);
                }
            }
        }
        catch (...) {}

        ImGui::Text("%s", currentPath.c_str());

        // Compute list size
        int pans_width  = ImGui::GetWindowWidth() * 0.5f - 12.0f;
        int pans_height = ImGui::GetWindowHeight() - 100.0f;

        // FOLDERS
        ImGui::BeginChild("Directories##1", ImVec2(pans_width, pans_height), 
                            true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Columns(2);

        // set first columns size
        static float initialSpacingColumn0_folders = pans_width * 0.8;
        if (initialSpacingColumn0_folders > 0) {
            ImGui::SetColumnWidth(0, initialSpacingColumn0_folders);
            initialSpacingColumn0_folders = 0.0f;
        }
        // Set column ordering
        if (ImGui::Selectable("Name")) {
            set_sort_order_by_name(folderNameSortOrder, folderDateSortOrder, 
                        folderSizeSortOrder, folderTypeSortOrder);
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Date")) {
            set_sort_order_by_date(folderNameSortOrder, folderDateSortOrder, 
                        folderSizeSortOrder, folderTypeSortOrder);
        }
        ImGui::NextColumn();
        ImGui::Separator();

        // SORTING folders
        if (folderNameSortOrder != SortOrder::None) {
            sort_by_name(folders, folderNameSortOrder);
        } else if (folderDateSortOrder != SortOrder::None) {
            sort_by_date(folders, folderDateSortOrder);
        }

        // Add reference to parent folder
        if (ImGui::Selectable("..", false, ImGuiSelectableFlags_AllowDoubleClick, 
                                ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
            if (ImGui::IsMouseDoubleClicked(0)) {
                if (!fs::path(currentPath).parent_path().string().empty()) {
                    currentPath = fs::path(currentPath).parent_path().string();
                }
            }
        }
        ImGui::NextColumn();
        ImGui::TextUnformatted("");
        ImGui::NextColumn();

        for (std::size_t i = 0; i < folders.size(); ++i) {
            std::string folder_name = folders[i].path().stem().string();
            if (ImGui::Selectable(folder_name.c_str(), (int)i == fileDialogFolderSelectIndex, 
                                ImGuiSelectableFlags_AllowDoubleClick, 
                                ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
                // Select a different folder so reset files
                currentFile = "";

                // change folder
                if (ImGui::IsMouseDoubleClicked(0)) {
                    currentPath = folders[i].path().string();
                    fileDialogFolderSelectIndex = 0;
                    ImGui::SetScrollHereY(0.0f);
                    currentFolder = "";
                    selected_files.fill(false);
                }
                else {
                    fileDialogFolderSelectIndex = i;
                    currentFolder = folders[i].path().stem().string();
                }
            }
            ImGui::NextColumn();
            std::string date = get_file_date(folders[i]);
            ImGui::TextUnformatted(date.c_str());
            ImGui::NextColumn();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("Files##1", ImVec2(pans_width, pans_height), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Columns(4);
        static float initialSpacingColumn0 = 230.0f;
        if (initialSpacingColumn0 > 0) {
            ImGui::SetColumnWidth(0, initialSpacingColumn0);
            initialSpacingColumn0 = 0.0f;
        }
        static float initialSpacingColumn1 = 80.0f;
        if (initialSpacingColumn1 > 0) {
            ImGui::SetColumnWidth(1, initialSpacingColumn1);
            initialSpacingColumn1 = 0.0f;
        }
        static float initialSpacingColumn2 = 80.0f;
        if (initialSpacingColumn2 > 0) {
            ImGui::SetColumnWidth(2, initialSpacingColumn2);
            initialSpacingColumn2 = 0.0f;
        }
        if (ImGui::Selectable("Name")) {
            set_sort_order_by_name(fileNameSortOrder, fileDateSortOrder, 
                        fileSizeSortOrder, fileTypeSortOrder);
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Size")) {
            set_sort_order_by_size(fileNameSortOrder, fileDateSortOrder, 
                        fileSizeSortOrder, fileTypeSortOrder);
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Type")) {
            set_sort_order_by_type(fileNameSortOrder, fileDateSortOrder, 
                        fileSizeSortOrder, fileTypeSortOrder);
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Date")) {
            set_sort_order_by_date(fileNameSortOrder, fileDateSortOrder, 
                        fileSizeSortOrder, fileTypeSortOrder);
        }
        ImGui::NextColumn();
        ImGui::Separator();

        // Sort files
        if (fileNameSortOrder != SortOrder::None) {
            sort_by_name(files, fileNameSortOrder);
        } else if (fileSizeSortOrder != SortOrder::None) {
            sort_by_size(files, fileSizeSortOrder);
        } else if (fileTypeSortOrder != SortOrder::None) {
            sort_by_type(files, fileTypeSortOrder);
        } else if (fileDateSortOrder != SortOrder::None) {
            sort_by_date(files, fileDateSortOrder);
        }

        for (std::size_t i = 0; i < files.size(); ++i) {
            std::string name = files[i].path().filename().string();
            ImGui::Selectable(name.c_str(), &selected_files[i], 
                    ImGuiSelectableFlags_AllowDoubleClick, 
                    ImVec2(ImGui::GetWindowContentRegionWidth(), 0));
            ImGui::NextColumn();
            ImGui::TextUnformatted(std::to_string(fs::file_size(files[i])).c_str());
            ImGui::NextColumn();
            ImGui::TextUnformatted(files[i].path().extension().string().c_str());
            ImGui::NextColumn();
            std::string date = get_file_date(files[i]);
            ImGui::TextUnformatted(date.c_str());
            ImGui::NextColumn();
        }
        ImGui::EndChild();


        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 140);
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 40);

        if (ImGui::Button("Choose")) {
            for (std::size_t i = 0; i < files.size(); i++) {
                if (selected_files[i]) {
                    selection.push_back(files[i].path().string());
                }
            }
            if (selection.size() > 0) {
                OpenFileDialog(false);
            }

        }
        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            selected_files.fill(false);
            fileDialogFolderSelectIndex = 0;
            currentFile = "";
            OpenFileDialog(false);
        }

        ImGui::End();

    }

    std::string get_file_date(fs::directory_entry &entry)
    {
        auto ftime = fs::last_write_time(entry);
        auto st = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
        std::time_t tt = std::chrono::system_clock::to_time_t(st);
        std::tm* mt = std::localtime(&tt);
        std::stringstream ss;
        ss << std::put_time(mt, "%F %R");
        return ss.str();

    }

    void sort_by_name(std::vector<fs::directory_entry> &list, SortOrder &order)
    {
        std::sort(list.begin(), list.end(), 
                [order](const fs::directory_entry& a, const fs::directory_entry& b) {
                    if (order == SortOrder::Down) {
                        return a.path().filename().string() > b.path().filename().string();
                    } else {
                        return a.path().filename().string() < b.path().filename().string();
                    }
                });
    }
    
    void sort_by_date(std::vector<fs::directory_entry> &list, SortOrder &order)
    {
        std::sort(list.begin(), list.end(), 
                [order](const fs::directory_entry& a, const fs::directory_entry& b) {
                    if (order == SortOrder::Down) {
                        return fs::last_write_time(a) > fs::last_write_time(b);
                    } else {
                        return fs::last_write_time(a) < fs::last_write_time(b);
                    }
                });
    }
    
    void sort_by_size(std::vector<fs::directory_entry> &list, SortOrder &order)
    {
        std::sort(list.begin(), list.end(), 
                [order](const fs::directory_entry& a, const fs::directory_entry& b) {
                    if (order == SortOrder::Down) {
                        return fs::file_size(a) > fs::file_size(b);
                    } else {
                        return fs::file_size(a) < fs::file_size(b);
                    }
                });
    }

    void sort_by_type(std::vector<fs::directory_entry> &list, SortOrder &order)
    {
        std::sort(list.begin(), list.end(), 
                [order](const fs::directory_entry& a, const fs::directory_entry& b) {
                    if (order == SortOrder::Down) {
                        return a.path().extension().string() > b.path().extension().string();
                    } else {
                        return a.path().extension().string() < b.path().extension().string();
                    }
                });
    }

    void set_sort_order_by_name(SortOrder &name, SortOrder &date, 
                        SortOrder &size, SortOrder &type)
    {
            size = SortOrder::None;
            date = SortOrder::None;
            type = SortOrder::None;
            if (name == SortOrder::Down) {
                name = SortOrder::Up;
            } else {
                name = SortOrder::Down;
            }
    }

    void set_sort_order_by_date(SortOrder &name, SortOrder &date, 
                    SortOrder &size, SortOrder &type)
    {
            name = SortOrder::None;
            type = SortOrder::None;
            size = SortOrder::None;
            if (date == SortOrder::Down) {
                date = SortOrder::Up;
            } else {
                date = SortOrder::Down;
            }
    }
    
    void set_sort_order_by_size(SortOrder &name, SortOrder &date, 
                    SortOrder &size, SortOrder &type)
    {
            name = SortOrder::None;
            date = SortOrder::None;
            type = SortOrder::None;
            if (size == SortOrder::Down) {
                size = SortOrder::Up;
            } else {
                size = SortOrder::Down;
            }
    }

    void set_sort_order_by_type(SortOrder &name, SortOrder &date, 
                    SortOrder &size, SortOrder &type)
    {
            name = SortOrder::None;
            date = SortOrder::None;
            size = SortOrder::None;
            if (type == SortOrder::Down) {
                type = SortOrder::Up;
            } else {
                type = SortOrder::Down;
            }
    }

} // namespace FileDialog
} // namespace UI
