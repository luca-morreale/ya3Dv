
#include "object_3d.hpp"

bool Object3D::compute_conformal;

Object3D::Object3D(std::string path) : path(path)
{
    this->data = std::shared_ptr<Data3D>(new Data3D);
    this->load_file();
    this->name = path.substr(path.find_last_of('/') + 1, path.find_last_of('.'));
}

Object3D::Object3D(std::string path, std::string name) : path(path), name(name)
{
    this->data = std::shared_ptr<Data3D>(new Data3D);
    this->load_file();
}

Object3D::~Object3D()
{ /**/ }

/*
 * File loading
 */
void Object3D::load_file()
{
    loadPolygonSoup(this->path, this->data);
    this->has_faces   = this->data->faceIndices.size() > 0;
    this->has_texture = this->data->textureCoords.size() > 0;
    this->generate_random_colors();
}

/*
 * Setters & getters
 */
void Object3D::set_points(std::vector<std::array<double, 3>> &points)
{
    this->data->vertices.clear();
    for (std::array<double, 3> point : points)
    {
        glm::vec3 converted_point = {point[0], point[1], point[2]};
        this->data->vertices.push_back(converted_point);
    }
}

void Object3D::set_vertices_random_colors(std::vector<std::array<double, 3>> &rand_colors)
{
    if (rand_colors.size() == this->data->vertices.size()) {
        this->v_rand_colors = rand_colors;
    }
}

std::vector<std::array<double, 3>> Object3D::get_vertices_random_colors()
{
    return this->v_rand_colors;
}

void Object3D::set_faces_random_colors(std::vector<std::array<double, 3>> &rand_colors)
{
    if (rand_colors.size() == this->data->faceIndices.size()) {
        this->f_rand_colors = rand_colors;
    }
}

std::vector<std::array<double, 3>> Object3D::get_faces_random_colors()
{
    return this->f_rand_colors;
}

bool Object3D::is_mesh()
{
    return this->has_faces;
}

/*
 * Shape draw
 */
void Object3D::draw()
{
    if (this->is_mesh()) {
        this->draw_mesh();
    } else {
        this->draw_point_cloud();
    }
}

void Object3D::draw_point_cloud()
{
    polyscope::registerPointCloud(this->name, this->data->vertices);
    polyscope::getPointCloud(this->name)->addColorQuantity("V random color", this->v_rand_colors);
}

void Object3D::draw_mesh()
{
    polyscope::registerSurfaceMesh(this->name, this->data->vertices, this->data->faceIndices);
    polyscope::getSurfaceMesh(this->name)->addFaceColorQuantity("F random color", this->f_rand_colors);

    for (auto attribute : this->data->vertexAttributes)
    {
        polyscope::getSurfaceMesh(this->name)->addVertexScalarQuantity(attribute.first.c_str(), attribute.second);
    }

    if (this->has_texture)
    {
        polyscope::getSurfaceMesh(this->name)->addVertexParameterizationQuantity("Parameterization", this->data->textureCoords, polyscope::ParamCoordsType::WORLD);
    }

}

/*
 * Generate random a color for each vertices
 */
void Object3D::generate_random_colors()
{
    for (uint i = 0; i < this->data->vertices.size(); i++)
    {
        std::array<double, 3> color = {polyscope::randomUnit(), polyscope::randomUnit(), polyscope::randomUnit()};
        this->v_rand_colors.push_back(color);
    }

    for (uint i = 0; i < this->data->faceIndices.size(); i++)
    {
        std::array<double, 3> color = {polyscope::randomUnit(), polyscope::randomUnit(), polyscope::randomUnit()};
        this->f_rand_colors.push_back(color);
    }
}
