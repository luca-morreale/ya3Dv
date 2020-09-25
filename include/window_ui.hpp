
#ifndef WINDOW_UI_HPP_
#define WINDOW_UI_HPP_

#define GLM_ENABLE_EXPERIMENTAL

#include <polyscope/polyscope.h>

#include <vector>
#include <array>
#include <map>

#include "filesystem.hpp"

#include "file_dialog.hpp"

namespace UI {

    // UI interface
    static int WINDOW_WIDTH = 500;
    static int MAIN_FRAME_WIDTH = 300;
    static int MAIN_FRAME_HEIGHT = 80;
    static bool open_file = false;
    static bool open_interpolation = false;


    // interpolation window
    static int INTERPOLATION_FRAME_WIDTH = 300;
    static int INTERPOLATION_FRAME_HEIGHT = 50;
    static bool animate = false;
    static float interpolation_slider = 0.0f;
    const static float SLIDER_MIN = 0.0;
    const static float SLIDER_MAX = 1.0;

    // open window
    static int OPEN_FRAME_WIDTH = 500;
    static int OPEN_FRAME_HEIGHT = 300;
    static int selected_folder = -1;
    static bool changed_folder = false;
    static std::vector<std::string> folders;
    static std::string folder_list_file = "../folders.txt";
    static std::string preview_folder = " ";

    static std::array<bool, 1000> selected_files;
    static std::vector<std::string> files;
    static std::map<std::string, std::vector<std::string>> interpolations;
    static std::array<bool, 1000> selected_interpolations;


    extern std::function<void(std::vector<std::string>&)> open_file_callback;
    extern std::function<void(std::map<std::string, std::vector<std::string>>&, std::array<bool, 1000>&)> open_interpolation_callback;
    extern std::function<void()> animate_interpolations;
    extern std::function<void(float)> set_interpolations_level;

    void callback();

    void interpolation_ui(ImVec2 &pos);
    void open_file_ui(ImVec2 &pos);
    void open_interpolation_ui(ImVec2 &pos);

    void folders_selection_ui();
    void files_selection_ui();
    void interpolation_selection_ui();

    void clear_cache();

    void populate_folders();
    void populate_files();
    void populate_interpolations();

    bool is_animatable_file(std::string file_path);
    std::string extract_interpolation_key(std::string file_path);


}




#endif // WINDOW_UI_HPP_
