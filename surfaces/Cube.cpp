#include "Cube.h"

Cube::Cube(QVector3D centerPosition, QVector3D scale) {
    initVertices(centerPosition, scale);
    initTriangles();
    initNeighbors();
}

void Cube::initVertices(QVector3D centerPosition, QVector3D scale) {
    float stepSize = scale.x();
    vertices[0] = {.index = 0, .position = QVector3D(stepSize,  -stepSize,  stepSize)};
    vertices[1] = {.index = 1, .position = QVector3D( stepSize,  stepSize,  stepSize)};
    vertices[2] = {.index = 2, .position = QVector3D(-stepSize,  stepSize,  stepSize)};
    vertices[3] = {.index = 3, .position = QVector3D(-stepSize,  -stepSize,  stepSize)};
    vertices[4] = {.index = 4, .position = QVector3D(stepSize,  -stepSize,  -stepSize)};
    vertices[5] = {.index = 5, .position = QVector3D(stepSize,  stepSize,  -stepSize)};
    vertices[6] = {.index = 6, .position = QVector3D(-stepSize,  stepSize,  -stepSize)};
    vertices[7] = {.index = 7, .position = QVector3D(-stepSize,  -stepSize,  -stepSize)};
}

void Cube::initTriangles() {
    triangles[0] = {{0, 1, 2}};
    triangles[1] = {{2, 3, 0}};
    triangles[2] = {{3, 2, 6}};
    triangles[3] = {{6, 7, 3}};
    triangles[4] = {{3, 7, 4}};
    triangles[5] = {{4, 0, 3}};
    triangles[6] = {{4, 7, 6}};
    triangles[7] = {{6, 5, 4}};
    triangles[8] = {{0, 4, 5}};
    triangles[9] = {{5, 1, 0}};
    triangles[10] = {{1, 5, 6}};
    triangles[11] = {{6, 2, 1}};
}

Triangle* Cube::getTriangles() {
    return triangles;
}

Vertex* Cube::getVertices() {
    return vertices;
}

int Cube::getNumVertices() {
    return numVertices;
}

int Cube::getNumTriangles() {
    return numTriangles;
}

