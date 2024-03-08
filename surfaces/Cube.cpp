#include "Cube.h"

Cube::Cube(QVector3D centerPosition, QVector3D scale) {
    initVertices(centerPosition, scale);
    initTriangles();
    initNeighborsAndNormals();
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
    triangles.push_back({{&vertices[0], &vertices[1], &vertices[2]}});
    triangles.push_back({{&vertices[2], &vertices[3], &vertices[0]}});
    triangles.push_back({{&vertices[3], &vertices[2], &vertices[6]}});
    triangles.push_back({{&vertices[6], &vertices[7], &vertices[3]}});
    triangles.push_back({{&vertices[3], &vertices[7], &vertices[4]}});
    triangles.push_back({{&vertices[4], &vertices[0], &vertices[3]}});
    triangles.push_back({{&vertices[4], &vertices[7], &vertices[6]}});
    triangles.push_back({{&vertices[6], &vertices[5], &vertices[4]}});
    triangles.push_back({{&vertices[0], &vertices[4], &vertices[5]}});
    triangles.push_back({{&vertices[5], &vertices[1], &vertices[0]}});
    triangles.push_back({{&vertices[1], &vertices[5], &vertices[6]}});
    triangles.push_back({{&vertices[6], &vertices[2], &vertices[1]}});
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

