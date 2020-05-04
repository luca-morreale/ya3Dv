
#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <array>

#include <args/args.hxx>
#include <polyscope/polyscope.h>

#include "object_3d.hpp"
#include "window_ui.hpp"
#include "interpolations.hpp"



std::vector<Object3D> visible_objects;
std::vector<AnimatedInterpolation> visible_interpolations;


void open_file_callback(std::vector<std::string> &files, std::array<bool, 1000> &selected);
void open_interpolation_callback(std::map<std::string, std::vector<std::string>> &files, std::array<bool, 1000> &selected);
void animate_interpolations();
void set_interpolations_level(float interpolation_factor);


int main(int argc, char** argv)
{
    // parse args
    args::ArgumentParser parser("Visualizer for 3D meshes and point cloud.");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Flag conformal(parser, "conformal", "Compute conformal measure", {'c', "conformal"});

    try {
        parser.ParseCLI(argc, argv);
        if (conformal) {
            Object3D::compute_conformal = true;
        } else {
            Object3D::compute_conformal = false;
        }
    } catch (const args::Help&) {
        std::cout << parser;
        return 0;
    } catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }


    // visualization

    polyscope::init();

    polyscope::state::userCallback = UI::callback;
    UI::open_file_callback = open_file_callback;
    UI::open_interpolation_callback = open_interpolation_callback;
    UI::animate_interpolations = animate_interpolations;
    UI::set_interpolations_level = set_interpolations_level;


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

void animate_interpolations()
{
    for (uint i = 0; i < visible_interpolations.size(); i++) {
        visible_interpolations[i].animate();
    }
}

void set_interpolations_level(float interpolation_factor)
{
    for (uint i = 0; i < visible_interpolations.size(); i++) {
        visible_interpolations[i].set_interpolation_factor(interpolation_factor);
    }
}
