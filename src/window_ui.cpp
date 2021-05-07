
#include "window_ui.hpp"

namespace UI {

    std::function<void(std::vector<std::string>&)> open_file_callback;
    std::function<void(std::vector<std::string>&)> open_interpolation_callback;
    std::function<void()> animate_interpolations;
    std::function<void(float)> set_interpolations_level;

    void callback()
    {
        ImVec2 pos = ImGui::GetWindowPos();

        ImGui::PushItemWidth(MAIN_FRAME_WIDTH);

        interpolation_ui(pos);

        ImGui::Separator();
        ImGui::Separator();

        if (ImGui::Button("Hi")) {
            polyscope::warning("Hi");
        }
        ImGui::SameLine();
        if (ImGui::Button("Open File")) {
            open_file = true;
            FileDialog::OpenFileDialog(true);
        }

        ImGui::SameLine();
        if (ImGui::Button("Open Interpolation")) {
            open_interpolation = true;
            FileDialog::OpenFileDialog(true);
        }

        ImGui::PopItemWidth();



        if (open_interpolation) {
            open_interpolation_ui(pos);
        }
        if (open_file) {
            open_file_ui(pos);
        }


        if (animate) {
            animate_interpolations();
        }

    }

    void interpolation_ui(ImVec2 &pos)
    {
        // check value is changed
        if (ImGui::SliderFloat("Interpolation Factor", &interpolation_slider, SLIDER_MIN, SLIDER_MAX)) {
            // TODO fix ticks by hand
            if (!animate) {
                set_interpolations_level(interpolation_slider);
            }
        }

        if (ImGui::Button("Animate")) {
            animate = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            animate = false;
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Animation Hz", &AnimatedInterpolation::MAX_WAIT_STEPS, 1, 5);
    }


    void open_file_ui(ImVec2 &pos)
    {

        std::vector<std::string> selection;
        if (FileDialog::IsFileDialogOpen()) {
            FileDialog::ShowFileDialog(selection);
        }

        open_file_callback(selection);

    }

    void open_interpolation_ui(ImVec2 &pos)
    {
        std::vector<std::string> selection;
        if (FileDialog::IsFileDialogOpen()) {
            FileDialog::ShowFileDialog(selection);
        }
        open_interpolation_callback(selection);
    }

    void files_selection_ui()
    {
        if (ImGui::ListBoxHeader("Files")) {
            populate_files();
            for (uint i = 0; i < files.size(); i++) {
                // remove folder name from visible path
                std::string name = files[i].substr(folders[selected_folder].length()+1,
                                        files[i].length());
                ImGui::Selectable(name.c_str(), &selected_files[i]);
            }
            ImGui::ListBoxFooter();
        }
    }


    void clear_cache()
    {
        preview_folder = " ";
        selected_folder = -1;
        selected_files.fill(false);
        folders.clear();
        files.clear();
        open_file = false;
        open_interpolation = false;

        interpolations.clear();
        selected_interpolations.fill(false);
    }


    void populate_folders()
    {
        if (folders.size() > 0) return;

        folders = read_folder_list(folder_list_file);
    }

    void populate_files()
    {
        if (files.size() > 0 || selected_folder < 0) return;

        files = list_files(folders[selected_folder]);
    }


    void populate_interpolations()
    {
        if (interpolations.size() > 0 || selected_folder < 0) return;

        std::vector<std::string> files = list_files(folders[selected_folder]);

        for (uint i = 0; i < files.size(); i++) {
            if (is_animatable_file(files[i])) {
                std::string key = extract_interpolation_key(files[i]);

                if (interpolations.find(key) == interpolations.end()) {
                    interpolations[key] = std::vector<std::string>();
                }
                interpolations[key].push_back(files[i]);
            }
        }
    }

    /**
     * Checks if contains '[' and ']' chars.
     */
    bool is_animatable_file(std::string file_path)
    {
        return file_path.find('[') != std::string::npos &&
                file_path.find(']') != std::string::npos;
    }

    std::string extract_interpolation_key(std::string file_path)
    {
        std::string current_file = file_path.substr(file_path.rfind("/") + 1);
        std::string suffix = current_file.substr(current_file.rfind("[") + 1);
        std::string key = suffix.substr(0, suffix.rfind("]"));
        return key;
    }



} // namespace UI
