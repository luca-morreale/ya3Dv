#ifndef OBJECT_3D_HPP_
#define OBJECT_3D_HPP_

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <array>
#include <memory>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <polyscope/polyscope.h>
#include <polyscope/point_cloud.h>
#include <polyscope/surface_mesh.h>
#include <polyscope/surface_mesh_io.h>

#include "mesh_io.hpp"
#include "data3d.hpp"

class Object3D {

public:
    Object3D(std::string path);
    Object3D(std::string path, std::string name);
    ~Object3D();

    void draw();
    void draw_point_cloud();
    void draw_mesh();

    void set_points(std::vector<std::array<double, 3>> &points);
    void set_vertices_random_colors(std::vector<std::array<double, 3>> &rand_colors);
    std::vector<std::array<double, 3>> get_vertices_random_colors();

    void set_faces_random_colors(std::vector<std::array<double, 3>> &rand_colors);
    std::vector<std::array<double, 3>> get_faces_random_colors();

    bool is_mesh();

    static bool compute_conformal;

protected:
    void load_file();

    void generate_random_colors();


private:
    std::string path;
    std::string name;

    Data3DPtr data;

    std::vector<std::array<double, 3>> v_rand_colors;
    std::vector<std::array<double, 3>> f_rand_colors;

    bool has_faces;
    bool has_texture;

};


#endif // OBJECT_3D_HPP_
