
#include "object_3d.hpp"

bool Object3D::compute_conformal;

Object3D::Object3D(std::string path) : path(path)
{
    this->load_file();
    this->name = path.substr(path.find_last_of('/') + 1, path.find_last_of('.'));
}

Object3D::Object3D(std::string path, std::string name) : path(path), name(name)
{
    this->load_file();
}

Object3D::~Object3D()
{
    this->points.clear();
    this->faces.clear();
}

/*
 * File loading
 */
void Object3D::load_file()
{
    loadPolygonSoup(this->path, this->points, this->faces, this->text_points);
    this->generate_random_colors();
    this->has_faces = this->faces.size() > 0;
    this->has_texture = this->text_points.size() > 0;

    if (this->is_mesh()) {
        this->compute_mesh_property();
    }
}

/*
 * Setters & getters
 */
void Object3D::set_points(std::vector<std::array<double, 3>> &points)
{
    for (std::array<double, 3> point : points)
    {
        glm::vec3 converted_point = {point[0], point[1], point[2]};
        this->points.push_back(converted_point);
    }
}

void Object3D::set_vertices_random_colors(std::vector<std::array<double, 3>> &rand_colors)
{
    this->v_rand_colors = rand_colors;
}

std::vector<std::array<double, 3>> Object3D::get_vertices_random_colors()
{
    return this->v_rand_colors;
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
    polyscope::registerPointCloud(this->name, this->points);
    polyscope::getPointCloud(this->name)->addColorQuantity("V random color", this->v_rand_colors);
}

void Object3D::draw_mesh()
{
    polyscope::registerSurfaceMesh(this->name, this->points, this->faces);
    // polyscope::getSurfaceMesh(this->name)->addFaceVectorQuantity("F normals", this->f_normals);
    polyscope::getSurfaceMesh(this->name)->addVertexColorQuantity("V random color", this->v_rand_colors);
    // polyscope::getSurfaceMesh(this->name)->addVertexScalarQuantity("G curvature", this->v_curv, polyscope::DataType::MAGNITUDE);
    // polyscope::getSurfaceMesh(this->name)->addVertexVectorQuantity("V normals", this->v_normals);

    if (this->has_texture) {
        polyscope::getSurfaceMesh(this->name)->addVertexParameterizationQuantity("Parameterization", this->text_points);
    }

    if (this->f_conformality.size() > 0){
        polyscope::getSurfaceMesh(this->name)->addFaceScalarQuantity("F conformality", this->f_conformality);
        // polyscope::getSurfaceMesh(this->name)->addFaceScalarQuantity("F shrinkage", this->f_shrinkage);
    }

}

/*
 * Generate random a color for each vertices
 */
void Object3D::generate_random_colors()
{
    for (uint i = 0; i < this->points.size(); i++) {
        std::array<double, 3> color = {polyscope::randomUnit(), polyscope::randomUnit(), polyscope::randomUnit()};
        this->v_rand_colors.push_back(color);
    }
}


/*
 * Mesh property estimation
 */
void Object3D::compute_mesh_property()
{
    Eigen::MatrixXd V = this->eigen_points();
    Eigen::MatrixXd T = this->eigen_texture();
    Eigen::MatrixXi F = this->eigen_faces();

    Eigen::MatrixXd N_vertices;
    Eigen::MatrixXd N_faces;
    Eigen::VectorXd K;

    // Compute per-face normals
    igl::per_face_normals(V, F, N_faces);
    // Compute per-vertex normals
    igl::per_vertex_normals(V, F, N_vertices);
    // Compute integral of Gaussian curvature
    igl::gaussian_curvature(V, F, K);

    // transfer data in parallel
    #pragma omp parallel sections
    {
        #pragma omp section
        convert_vector(N_faces, this->f_normals);
        #pragma omp section
        convert_vector(N_vertices, this->v_normals);
        #pragma omp section
        convert_vector(K, this->v_curv);
    }

    if (Object3D::compute_conformal){
        Eigen::VectorXd conformality(this->faces.size());
        Eigen::VectorXd shrinkage(this->faces.size());
        this->compute_mesh_conformality(V, T, conformality, shrinkage);
        // transfer data in parallel
        #pragma omp parallel sections
        {
            #pragma omp section
            convert_vector(conformality, this->f_conformality);
            #pragma omp section
            convert_vector(shrinkage, this->f_shrinkage);
        }
    }

}

void Object3D::compute_mesh_conformality(Eigen::MatrixXd &V, Eigen::MatrixXd &T,
                                        Eigen::VectorXd &conformality, Eigen::VectorXd &shrinkage)
{
    Eigen::Vector2f cols;
    cols << 0, 1;

    for (uint i = 0; i < this->faces.size(); i++) {

        Eigen::MatrixXd P;
        Eigen::Matrix3d Q;
        Eigen::Matrix<double, 3,2> M;
        Eigen::Vector3d rows;

        std::vector<size_t> face = this->faces[i];
        rows << face[0], face[1], face[2];

        igl::slice(V, rows, 1, Q); // 1 is to say across which dimension
        igl::slice(T, rows, 1, P);

        // extends P of 1 dimension
        P.conservativeResize(3,3);
        P.col(2).setOnes();

        Eigen::Matrix3d x = P.colPivHouseholderQr().solve(Q).transpose();

        igl::slice(x, cols, 2, M);
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(M);
        Eigen::Vector2d eigv = svd.singularValues();

        conformality(i) = eigv(0) / eigv(1);
        shrinkage(i)    = eigv(0) * eigv(1);
    }

}


/*
 * Convert internal data to Eigen data type
 */
Eigen::MatrixXd Object3D::eigen_points()
{
    Eigen::MatrixXd V(this->points.size(), 3);
    #pragma omp parallel for
    for (uint i = 0 ; i < this->points.size(); i++) {
        V(i, 0) = (double)this->points[i].x;
        V(i, 1) = (double)this->points[i].y;
        V(i, 2) = (double)this->points[i].z;
    }
    return V;
}

Eigen::MatrixXd Object3D::eigen_texture()
{
    Eigen::MatrixXd T(this->text_points.size(), 2);
    #pragma omp parallel for
    for (uint i = 0 ; i < this->text_points.size(); i++) {
        T(i, 0) = (double)this->text_points[i][0];
        T(i, 1) = (double)this->text_points[i][1];
    }
    return T;
}

Eigen::MatrixXi Object3D::eigen_faces()
{
    Eigen::MatrixXi F(this->faces.size(), 3);
    #pragma omp parallel for
    for (uint i = 0 ; i < this->faces.size(); i++) {
        F(i, 0) = (int)this->faces[i][0];
        F(i, 1) = (int)this->faces[i][1];
        F(i, 2) = (int)this->faces[i][2];
    }
    return F;
}


/*
 * Convert Eigen data to vector type
 */
void Object3D::convert_vector(Eigen::MatrixXd &input_data, std::vector<glm::vec3> &vector)
{
    vector.resize(input_data.rows());
    #pragma omp parallel for
    for (uint i = 0 ; i < input_data.rows(); i++) {
        vector[i][0] = (float)input_data(i,0);
        vector[i][1] = (float)input_data(i,1);
        vector[i][2] = (float)input_data(i,2);
    }
}

void Object3D::convert_vector(Eigen::VectorXd &input_data, std::vector<double> &vector)
{
    vector.resize(input_data.rows());
    #pragma omp parallel for
    for (uint i = 0 ; i < input_data.rows(); i++) {
        vector[i] = (float)input_data(i);
    }
}
