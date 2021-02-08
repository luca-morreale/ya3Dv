#ifndef DATA_3D_HPP_
#define DATA_3D_HPP_

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <array>
#include <unordered_map>
#include <memory>


typedef struct Data3D
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> vertNormals;
    std::vector<std::vector<size_t>> faceIndices;
    std::vector<std::array<double, 2>> textureCoords;
    std::unordered_map<std::string, std::vector<double> > vertexAttributes;
    std::unordered_map<std::string, std::vector<double> > faceAttributes;
    std::unordered_map<std::string, std::vector<std::array<double, 3>> > vertexVecAttributes;

    void set_vertices(std::vector<std::array<double, 3>> &in_vertex)
    {
        vertices.clear();
        for (uint i = 0; i < in_vertex.size(); i++) {
            vertices.push_back(glm::vec3(in_vertex[i][0], in_vertex[i][1], in_vertex[i][2]));
        }
    }

} Data3D;


typedef std::shared_ptr<Data3D> Data3DPtr;

#endif // DATA_3D_HPP_
