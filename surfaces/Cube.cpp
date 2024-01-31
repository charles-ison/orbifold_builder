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

