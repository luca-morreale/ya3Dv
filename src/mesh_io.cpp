
#include "mesh_io.hpp"

// code taken from polyscope
namespace polyscope {
namespace io {

    class Index {
        public:
            Index() {}

            Index(int v, int vt, int vn) : position(v), uv(vt), normal(vn) {}

            bool operator<(const Index& i) const {
                if (position < i.position) return true;
                if (position > i.position) return false;
                if (uv < i.uv) return true;
                if (uv > i.uv) return false;
                if (normal < i.normal) return true;
                if (normal > i.normal) return false;

                return false;
            }

            int position;
            int uv;
            int normal;
    };

    Index parseFaceIndex(const std::string& token) {
        std::stringstream in(token);
        std::string indexString;
        int indices[3] = {0, 0, 0};

        int i = 0;
        while (std::getline(in, indexString, '/')) {
            if (indexString != "\\") {
                std::stringstream ss(indexString);
                ss >> indices[i++];
            }
        }

        // decrement since indices in OBJ files are 1-based
        return Index(indices[0] - 1, indices[1] - 1, indices[2] - 1);
    }

} // namespace io
} // namespace polyscope


void load_OBJ(std::string filename,
                std::vector<std::array<double, 3>> &vertexPositionsOut,
                std::vector<std::vector<size_t>> &faceIndicesOut,
                std::vector<std::array<double, 2>> &texturePositionsOut)
{
    // Open the file
    std::ifstream in(filename);
    if (!in) throw std::invalid_argument("Could not open mesh file " + filename);

    // create temporary variables
    std::map<size_t, size_t> textureVertMapping;
    std::vector<std::array<double, 2>> tmpTextPositions;

    // reset output containers
    faceIndicesOut.clear();
    vertexPositionsOut.clear();
    texturePositionsOut.clear();


    // parse obj format
    std::string line;
    while (getline(in, line)) {
        std::stringstream ss(line);
        std::string token;

        ss >> token;

        if (token == "v") {         // vertex
            double x, y, z;
            ss >> x >> y >> z;
            vertexPositionsOut.push_back({{x, y, z}});

        } else if (token == "vt") { // texture vertex
            double u, v;
            ss >> u >> v;
            tmpTextPositions.push_back({{u, v}});

        } else if (token == "vn") { // vertex normal
            // Do nothing

        } else if (token == "f") {  // face

            std::vector<size_t> face;
            while (ss >> token) {
                polyscope::io::Index index = polyscope::io::parseFaceIndex(token);
                if (index.position < 0) {
                    getline(in, line);
                    size_t i = line.find_first_not_of("\t\n\v\f\r ");
                    index = polyscope::io::parseFaceIndex(line.substr(i));
                }

                face.push_back(index.position);
                textureVertMapping[index.uv] = index.position;
            }

            faceIndicesOut.push_back(face);
        }
    }

    texturePositionsOut.resize(vertexPositionsOut.size());
    for (auto it = textureVertMapping.begin(); it != textureVertMapping.end(); it++) {
        texturePositionsOut[it->second] = tmpTextPositions[it->first];
    }

}


void loadPolygonSoup(std::string filename,
                    std::vector<glm::vec3> &vertexPositionsOut,
                    std::vector<std::vector<size_t>> &faceIndicesOut,
                    std::vector<std::array<double, 2>> &texturePositionsOut)
{

    // Check if file exists
    std::ifstream testStream(filename);
    if (!testStream) {
        polyscope::error("Could not load polygon soup; file does not exist: " + filename);
        return;
    }
    testStream.close();

    // Attempt to detect filename
    std::string::size_type sepInd = filename.rfind('.');
    std::string extension = "";
    if (sepInd != std::string::npos) {
        extension = filename.substr(sepInd + 1);

        // Convert to all lowercase
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }

    std::vector<std::array<double, 3>> vertices;

    if (extension == "obj") {
        load_OBJ(filename, vertices, faceIndicesOut, texturePositionsOut);
    } else if (extension == "ply") {
        polyscope::loadPolygonSoup_PLY(filename, vertices, faceIndicesOut);
    } else {
        polyscope::error("Could not detect file type to load mesh from " + filename);
    }

    vertexPositionsOut.clear();
    for (uint i = 0; i < vertices.size(); i++) {
        vertexPositionsOut.push_back(glm::vec3(vertices[i][0], vertices[i][1], vertices[i][2]));
    }

}
