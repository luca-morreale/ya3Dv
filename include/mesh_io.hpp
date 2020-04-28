#ifndef LOAD_OBJ_HPP_
#define LOAD_OBJ_HPP_

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <polyscope/surface_mesh_io.h>
#include <polyscope/messages.h>

void load_OBJ(std::string filename,
            std::vector<std::array<double, 3>> &vertexPositionsOut,
            std::vector<std::vector<size_t>> &faceIndicesOut,
            std::vector<std::array<double, 2>> &texturePositionsOut);

void loadPolygonSoup(std::string filename,
                    std::vector<glm::vec3> &vertexPositionsOut,
                    std::vector<std::vector<size_t>> &faceIndicesOut,
                    std::vector<std::array<double, 2>> &texturePositionsOut);


#endif // LOAD_OBJ_HPP_
