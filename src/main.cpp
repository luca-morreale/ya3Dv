
#define GLM_ENABLE_EXPERIMENTAL

#include <polyscope/polyscope.h>

#include "object_3d.hpp"
#include "window_ui.hpp"
#include "interpolations.hpp"

#include <vector>
#include <array>

std::vector<Object3D> visible_objects;
std::vector<AnimatedInterpolation> visible_interpolations;

void open_file_callback(std::vector<std::string> &files, std::array<bool, 1000> &selected);
void open_interpolation_callback(std::map<std::string, std::vector<std::string>> &files, std::array<bool, 1000> &selected);
static void animate_interpolations();


int main(int argc, char** argv) {

    polyscope::init();

    polyscope::state::userCallback = UI::callback;
    UI::open_file_callback = open_file_callback;
    UI::open_interpolation_callback = open_interpolation_callback;
    UI::animate_interpolations = animate_interpolations;

    polyscope::show();

    return 0;
}


void open_file_callback(std::vector<std::string> &files, std::array<bool, 1000> &selected)
{
    for (uint i = 0; i < files.size(); i++) {
        if (selected[i]) {
            Object3D obj(files[i]);
            obj.draw();
            visible_objects.push_back(obj);
        }
    }
}


void open_interpolation_callback(std::map<std::string, std::vector<std::string>> &files, std::array<bool, 1000> &selected)
{
    // TODO fill this
    int i = 0;
    for (auto it = files.begin(); it != files.end(); it++) {
        if (selected[i]) {
            AnimatedInterpolation obj(it->second, it->first);
            obj.draw();
            visible_interpolations.push_back(obj);
        }
        i++;
    }
}

static void animate_interpolations()
{
    for (uint i = 0; i < visible_interpolations.size(); i++) {
        visible_interpolations[i].animate();
    }

}
