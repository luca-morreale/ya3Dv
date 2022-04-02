
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


void open_file_callback(std::vector<std::string> &files);
void open_interpolation_callback(std::vector<std::string> &files);
void animate_interpolations();
void set_interpolations_level(float interpolation_factor);


int main(int argc, char** argv)
{
    // parse args
    args::ArgumentParser parser("Visualizer for 3D meshes and point cloud.");
    args::PositionalList<std::string> meshes(parser, "mesh", "One or more files to visualize");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlagList<std::string> interps(parser, "interpolation", "One or more interpolations to visualize", {'i', "interp"});

    try {
        parser.ParseCLI(argc, argv);

    } catch (const args::Help&) {
        std::cout << parser;
        return 0;
    } catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    polyscope::options::programName = "ya3Dv";

    // visualization
    polyscope::view::upDir = polyscope::view::UpDir::YUp;
    polyscope::view::style = polyscope::view::NavigateStyle::Free;
    polyscope::options::groundPlaneMode = polyscope::GroundPlaneMode::None;
    polyscope::init();

    polyscope::state::userCallback = UI::callback;
    UI::open_file_callback = open_file_callback;
    UI::open_interpolation_callback = open_interpolation_callback;
    UI::animate_interpolations = animate_interpolations;
    UI::set_interpolations_level = set_interpolations_level;


    std::vector<std::string> meshes_list = meshes.Get();
    if (meshes_list.size() > 0) {
        open_file_callback(meshes_list);
    }
    std::vector<std::string> interps_list = interps.Get();
    if (interps_list.size() > 0) {
        open_interpolation_callback(interps_list);
    }

    polyscope::show();

    return 0;
}


void open_file_callback(std::vector<std::string> &files)
{
    for (uint i = 0; i < files.size(); i++) {
        Object3D obj(files[i]);
        obj.draw();
        visible_objects.push_back(obj);
    }
}


void open_interpolation_callback(std::vector<std::string> &files)
{
    for (uint i = 0; i < files.size(); i++) {
        AnimatedInterpolation obj(files[i]);
        obj.draw();
        visible_interpolations.push_back(obj);
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
