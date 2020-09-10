#ifndef LOAD_OBJ_HPP_
#define LOAD_OBJ_HPP_

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "happly/happly.h"

#include <polyscope/surface_mesh_io.h>
#include <polyscope/messages.h>

#include "data3d.hpp"

void load_OBJ(std::string filename, Data3DPtr data);

void load_PLY(std::string filename, Data3DPtr data);

void loadPolygonSoup(std::string filename, Data3DPtr data);


#endif // LOAD_OBJ_HPP_
