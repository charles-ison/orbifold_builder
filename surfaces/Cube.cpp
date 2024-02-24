#include "Cube.h"

Cube::Cube(QVector3D centerPosition, QVector3D scale) {
    initVertices(centerPosition, scale);
    initTriangles();
    initNeighbors();
}

void Cube::initVertices(QVector3D centerPosition, QVector3D scale) {
    float stepSize = scale.x();
    vertices.push_back({.index = 0, .position = QVector3D(stepSize,  -stepSize,  stepSize)});
    vertices.push_back({.index = 1, .position = QVector3D( stepSize,  stepSize,  stepSize)});
    vertices.push_back({.index = 2, .position = QVector3D(-stepSize,  stepSize,  stepSize)});
    vertices.push_back({.index = 3, .position = QVector3D(-stepSize,  -stepSize,  stepSize)});
    vertices.push_back({.index = 4, .position = QVector3D(stepSize,  -stepSize,  -stepSize)});
    vertices.push_back({.index = 5, .position = QVector3D(stepSize,  stepSize,  -stepSize)});
    vertices.push_back({.index = 6, .position = QVector3D(-stepSize,  stepSize,  -stepSize)});
    vertices.push_back({.index = 7, .position = QVector3D(-stepSize,  -stepSize,  -stepSize)});
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

Triangle* Cube::getTriangles() {
    return &triangles[0];
}

Vertex* Cube::getVertices() {
    return &vertices[0];
}

int Cube::getNumVertices() {
    return vertices.size();
}

int Cube::getNumTriangles() {
    return triangles.size();
}

