#ifndef OBJECT_3D_HPP_
#define OBJECT_3D_HPP_

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <array>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <polyscope/polyscope.h>
#include <polyscope/point_cloud.h>
#include <polyscope/surface_mesh.h>
#include <polyscope/surface_mesh_io.h>

#include <igl/gaussian_curvature.h>
#include <igl/per_vertex_normals.h>
#include <igl/per_face_normals.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/eigs.h>
#include <igl/slice.h>
#include <igl/slice_into.h>


#include "mesh_io.hpp"

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

    bool is_mesh();


protected:
    void load_file();
    void compute_mesh_property();
    void compute_mesh_conformality(Eigen::MatrixXd &V, Eigen::MatrixXd &T,
                                    Eigen::VectorXd &conformality, Eigen::VectorXd &shrinkage);

    void generate_random_colors();

    Eigen::MatrixXd eigen_points();
    Eigen::MatrixXd eigen_texture();
    Eigen::MatrixXi eigen_faces();
    void convert_vector(Eigen::MatrixXd &input_data, std::vector<glm::vec3> &vector);
    void convert_vector(Eigen::VectorXd &input_data, std::vector<double> &vector);

private:
    std::string path;
    std::string name;

    std::vector<glm::vec3> points;
    std::vector<std::vector<size_t>> faces;
    std::vector<std::array<double, 2>> text_points;

    std::vector<glm::vec3> v_normals;
    std::vector<glm::vec3> f_normals;
    std::vector<std::array<double, 3>> v_rand_colors;
    std::vector<double> v_curv;
    std::vector<double> f_conformality;
    std::vector<double> f_shrinkage;

    bool has_faces;
    bool has_texture;

};


#endif // OBJECT_3D_HPP_
