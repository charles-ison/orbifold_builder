#include "Cube.h"

Cube::Cube() {
    initVertices();
    initTriangles();
    initNeighbors();
}

void Cube::initVertices() {
    vertices[0] = {QVector3D(1.0f,  -1.0f,  1.0f)};
    vertices[1] = {QVector3D( 1.0f,  1.0f,  1.0f)};
    vertices[2] = {QVector3D(-1.0f,  1.0f,  1.0f)};
    vertices[3] = {QVector3D(-1.0f,  -1.0f,  1.0f)};
    vertices[4] = {QVector3D(1.0f,  -1.0f,  -1.0f)};
    vertices[5] = {QVector3D(1.0f,  1.0f,  -1.0f)};
    vertices[6] = {QVector3D(-1.0f,  1.0f,  -1.0f)};
    vertices[7] = {QVector3D(-1.0f,  -1.0f,  -1.0f)};
}

void Cube::initTriangles() {
    triangles.push_back({{0, 1, 2}});
    triangles.push_back({{2, 3, 0}});
    triangles.push_back({{3, 2, 6}});
    triangles.push_back({{6, 7, 3}});
    triangles.push_back({{3, 7, 4}});
    triangles.push_back({{4, 0, 3}});
    triangles.push_back({{4, 7, 6}});
    triangles.push_back({{6, 5, 4}});
    triangles.push_back({{0, 4, 5}});
    triangles.push_back({{5, 1, 0}});
    triangles.push_back({{1, 5, 6}});
    triangles.push_back({{6, 2, 1}});
}

std::vector<Triangle> Cube::getTriangles() {
    return triangles;
}

Vertex* Cube::getVertices() {
    return vertices;
}

int Cube::getNumVertices() {
    return numVertices;
}
