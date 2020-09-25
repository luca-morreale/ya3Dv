
#include "file_dialog.hpp"

namespace UI {

    bool fileDialogOpen = false;

    void ShowFileDialog(std::vector<std::string> &selection)
    {
        static std::array<bool, 1000> selected_files;
        static int fileDialogFolderSelectIndex = 0;
        static std::string fileDialogCurrentPath = ".";
        static std::string fileDialogCurrentFile = "";
        static std::string fileDialogCurrentFolder = "";
        static char fileDialogError[500] = "";
        static FileDialogSortOrder fileNameSortOrder = FileDialogSortOrder::None;
        static FileDialogSortOrder sizeSortOrder = FileDialogSortOrder::None;
        static FileDialogSortOrder dateSortOrder = FileDialogSortOrder::None;
        static FileDialogSortOrder typeSortOrder = FileDialogSortOrder::None;

        static FileDialogSortOrder folderNameSortOrder = FileDialogSortOrder::None;
        static FileDialogSortOrder folderSizeSortOrder = FileDialogSortOrder::None;
        static FileDialogSortOrder folderDateSortOrder = FileDialogSortOrder::None;
        static FileDialogSortOrder folderTypeSortOrder = FileDialogSortOrder::None;

        // converts initial relative path to absolute
        fileDialogCurrentPath = fs::canonical(fs::path(fileDialogCurrentPath)).string();


        // ImGui::SetNextWindowSize(ImVec2(740.0f, 395.0f));
        ImGui::Begin("Select a file", nullptr);

        std::vector<fs::directory_entry> files;
        std::vector<fs::directory_entry> folders;
        try {
            for (auto& p : fs::directory_iterator(fileDialogCurrentPath)) {
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

        ImGui::Text("%s", fileDialogCurrentPath.c_str());

        // ImGui::GetWindowWidth()
        int pans_width  = ImGui::GetWindowWidth() * 0.5f - 12.0f;
        int pans_height = ImGui::GetWindowHeight() - 100.0f;

        // TODO add sorting in folder

        ImGui::BeginChild("Directories##1", ImVec2(pans_width, pans_height), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Columns(2);
        static float initialSpacingColumn0_folders = pans_width * 0.8;
        if (initialSpacingColumn0_folders > 0) {
            ImGui::SetColumnWidth(0, initialSpacingColumn0_folders);
            initialSpacingColumn0_folders = 0.0f;
        }
        if (ImGui::Selectable("Name")) {
            folderSizeSortOrder = FileDialogSortOrder::None;
            folderDateSortOrder = FileDialogSortOrder::None;
            folderTypeSortOrder = FileDialogSortOrder::None;
            folderNameSortOrder = (folderNameSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Date")) {
            folderNameSortOrder = FileDialogSortOrder::None;
            folderSizeSortOrder = FileDialogSortOrder::None;
            folderTypeSortOrder = FileDialogSortOrder::None;
            folderDateSortOrder = (folderDateSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
        }
        ImGui::NextColumn();
        ImGui::Separator();

        // SORTING folders

        // Sort files
        if (folderNameSortOrder != FileDialogSortOrder::None) {
            std::sort(folders.begin(), folders.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
                if (folderNameSortOrder == FileDialogSortOrder::Down) {
                    return a.path().filename().string() > b.path().filename().string();
                }
                else {
                    return a.path().filename().string() < b.path().filename().string();
                }
                });
        }
        else if (folderDateSortOrder != FileDialogSortOrder::None) {
            std::sort(folders.begin(), folders.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
                if (folderDateSortOrder == FileDialogSortOrder::Down) {
                    return fs::last_write_time(a) > fs::last_write_time(b);
                }
                else {
                    return fs::last_write_time(a) < fs::last_write_time(b);
                }
                });
        }


        if (ImGui::Selectable("..", false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
            if (ImGui::IsMouseDoubleClicked(0)) {
                if (!fs::path(fileDialogCurrentPath).parent_path().string().empty()) {
                    fileDialogCurrentPath = fs::path(fileDialogCurrentPath).parent_path().string();
                }
            }
        }
        ImGui::NextColumn();
        ImGui::TextUnformatted("");
        ImGui::NextColumn();

        for (std::size_t i = 0; i < folders.size(); ++i) {
            if (ImGui::Selectable(folders[i].path().stem().string().c_str(), (int)i == fileDialogFolderSelectIndex, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
                fileDialogCurrentFile = "";

                // change folder
                if (ImGui::IsMouseDoubleClicked(0)) {
                    fileDialogCurrentPath = folders[i].path().string();
                    fileDialogFolderSelectIndex = 0;
                    ImGui::SetScrollHereY(0.0f);
                    fileDialogCurrentFolder = "";
                    selected_files.fill(false);
                }
                else {
                    fileDialogFolderSelectIndex = i;
                    fileDialogCurrentFolder = folders[i].path().stem().string();
                }
            }
            ImGui::NextColumn();
            auto ftime = fs::last_write_time(folders[i]);
            auto st = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
            std::time_t tt = std::chrono::system_clock::to_time_t(st);
            std::tm* mt = std::localtime(&tt);
            std::stringstream ss;
            ss << std::put_time(mt, "%F %R");
            ImGui::TextUnformatted(ss.str().c_str());
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
            sizeSortOrder = FileDialogSortOrder::None;
            dateSortOrder = FileDialogSortOrder::None;
            typeSortOrder = FileDialogSortOrder::None;
            fileNameSortOrder = (fileNameSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Size")) {
            fileNameSortOrder = FileDialogSortOrder::None;
            dateSortOrder = FileDialogSortOrder::None;
            typeSortOrder = FileDialogSortOrder::None;
            sizeSortOrder = (sizeSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Type")) {
            fileNameSortOrder = FileDialogSortOrder::None;
            dateSortOrder = FileDialogSortOrder::None;
            sizeSortOrder = FileDialogSortOrder::None;
            typeSortOrder = (typeSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Date")) {
            fileNameSortOrder = FileDialogSortOrder::None;
            sizeSortOrder = FileDialogSortOrder::None;
            typeSortOrder = FileDialogSortOrder::None;
            dateSortOrder = (dateSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
        }
        ImGui::NextColumn();
        ImGui::Separator();

        // Sort files
        if (fileNameSortOrder != FileDialogSortOrder::None) {
            std::sort(files.begin(), files.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
                if (fileNameSortOrder == FileDialogSortOrder::Down) {
                    return a.path().filename().string() > b.path().filename().string();
                }
                else {
                    return a.path().filename().string() < b.path().filename().string();
                }
                });
        }
        else if (sizeSortOrder != FileDialogSortOrder::None) {
            std::sort(files.begin(), files.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
                if (sizeSortOrder == FileDialogSortOrder::Down) {
                    return fs::file_size(a) > fs::file_size(b);
                }
                else {
                    return fs::file_size(a) < fs::file_size(b);
                }
                });
        }
        else if (typeSortOrder != FileDialogSortOrder::None) {
            std::sort(files.begin(), files.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
                if (typeSortOrder == FileDialogSortOrder::Down) {
                    return a.path().extension().string() > b.path().extension().string();
                }
                else {
                    return a.path().extension().string() < b.path().extension().string();
                }
                });
        }
        else if (dateSortOrder != FileDialogSortOrder::None) {
            std::sort(files.begin(), files.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
                if (dateSortOrder == FileDialogSortOrder::Down) {
                    return fs::last_write_time(a) > fs::last_write_time(b);
                }
                else {
                    return fs::last_write_time(a) < fs::last_write_time(b);
                }
                });
        }

        for (std::size_t i = 0; i < files.size(); ++i) {

            ImGui::Selectable(files[i].path().filename().string().c_str(), &selected_files[i], ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0));
            ImGui::NextColumn();
            ImGui::TextUnformatted(std::to_string(fs::file_size(files[i])).c_str());
            ImGui::NextColumn();
            ImGui::TextUnformatted(files[i].path().extension().string().c_str());
            ImGui::NextColumn();
            auto ftime = fs::last_write_time(files[i]);
            auto st = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
            std::time_t tt = std::chrono::system_clock::to_time_t(st);
            std::tm* mt = std::localtime(&tt);
            std::stringstream ss;
            ss << std::put_time(mt, "%F %R");
            ImGui::TextUnformatted(ss.str().c_str());
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
                fileDialogOpen = false;
            }


        }
        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            selected_files.fill(false);
            fileDialogFolderSelectIndex = 0;
            fileDialogCurrentFile = "";
            fileDialogOpen = false;
        }

        if (strlen(fileDialogError) > 0) {
            ImGui::TextColored(ImColor(1.0f, 0.0f, 0.2f, 1.0f), "%s", fileDialogError);
        }

        ImGui::End();

    }

} // namespace UI
