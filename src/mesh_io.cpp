
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


void load_OBJ(std::string filename, Data3DPtr data)
{
    // Open the file
    std::ifstream in(filename);
    if (!in) throw std::invalid_argument("Could not open mesh file " + filename);

    // create temporary variables
    std::map<size_t, size_t> textureVertMapping;
    std::vector<std::array<double, 2>> tmpTextPositions;
    std::vector<std::array<double, 3>> tmpVertexPositions;

    // reset data containers
    data->faceIndices.clear();

    // parse obj format
    std::string line;
    while (getline(in, line)) {
        std::stringstream ss(line);
        std::string token;

        ss >> token;

        if (token == "v") {         // vertex
            double x, y, z;
            ss >> x >> y >> z;
            tmpVertexPositions.push_back({{x, y, z}});

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

            data->faceIndices.push_back(face);
        }
    }

    data->textureCoords.resize(tmpVertexPositions.size());
    for (auto it = textureVertMapping.begin(); it != textureVertMapping.end(); it++) {
        data->textureCoords[it->second] = tmpTextPositions[it->first];
    }

    data->set_vertices(tmpVertexPositions);

}


void load_PLY(std::string filename, Data3DPtr data)
{
    // Construct a data object by reading from file
    happly::PLYData plyIn(filename);

    // Get mesh-style data from the object
    std::vector<std::array<double, 3>> tmpVertexPositions = plyIn.getVertexPositions();
    data->set_vertices(tmpVertexPositions);
    data->faceIndices = plyIn.getFaceIndices<size_t>();

    // extract vertices attributes
    std::vector<std::string> propNames = plyIn.getElement("vertex").getPropertyNames();

    propNames.erase(std::remove(propNames.begin(), propNames.end(), std::string("x")), propNames.end());
    propNames.erase(std::remove(propNames.begin(), propNames.end(), std::string("y")), propNames.end());
    propNames.erase(std::remove(propNames.begin(), propNames.end(), std::string("z")), propNames.end());

    for (std::string name : propNames)
    {
        std::vector<double> propData = plyIn.getElement("vertex").getProperty<double>(name.c_str());
        data->vertexAttributes[name] = propData;
    }

    // Fill in texture coords from attributes
    // has texture coords in s and t
    if (data->vertexAttributes.find("s") != data->vertexAttributes.end() &&
            data->vertexAttributes.find("t") != data->vertexAttributes.end())
    {
        data->textureCoords.resize(data->vertexAttributes["s"].size());
        for (uint i = 0; i < data->vertexAttributes["s"].size(); i++)
        {
            data->textureCoords[i] = { data->vertexAttributes["s"][i], data->vertexAttributes["t"][i] };
        }

    // has texture coords in texture_u and texture_v
    } else if (data->vertexAttributes.find("texture_u") != data->vertexAttributes.end() &&
                data->vertexAttributes.find("texture_v") != data->vertexAttributes.end())
    {
        data->textureCoords.resize(data->vertexAttributes["texture_u"].size());
        for (uint i = 0; i < data->vertexAttributes["texture_u"].size(); i++)
        {
            data->textureCoords[i] = { data->vertexAttributes["texture_u"][i],
                                        data->vertexAttributes["texture_v"][i] };
        }
    }

    data->vertexAttributes.erase("s");
    data->vertexAttributes.erase("t");
    data->vertexAttributes.erase("texture_u");
    data->vertexAttributes.erase("texture_v");

    // face attributes
}


void loadPolygonSoup(std::string filename, Data3DPtr data)
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

    if (extension == "obj") {
        load_OBJ(filename, data);
    } else if (extension == "ply") {
        // polyscope::loadPolygonSoup_PLY(filename, vertices, faceIndicesOut);
        load_PLY(filename, data);
    } else {
        polyscope::error("Could not detect file type to load mesh from " + filename);
    }

}
