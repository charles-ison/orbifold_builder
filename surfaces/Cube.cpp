#include "Cube.h"

Cube::Cube() {
    initVertices();
    initIndices();
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

void Cube::initIndices() {
    indices[0] = 0; indices[1] = 1; indices[2] = 2; indices[3] = 2; indices[4] = 3; indices[5] = 0;
    indices[6] = 3; indices[7] = 2; indices[8] = 6; indices[9] = 6; indices[10] = 7; indices[11] = 3;
    indices[12] = 3; indices[13] = 7; indices[14] = 4; indices[15] = 4; indices[16] = 0; indices[17] = 3;
    indices[18] = 4; indices[19] = 7; indices[20] = 6; indices[21] = 6; indices[22] = 5; indices[23] = 4;
    indices[24] = 0; indices[25] = 4; indices[26] = 5; indices[27] = 5; indices[28] = 1; indices[29] = 0;
    indices[30] = 1; indices[31] = 5; indices[32] = 6; indices[33] = 6; indices[34] = 2; indices[35] = 1;
}

VertexData* Cube::getVertices() {
    return vertices;
}

GLushort* Cube::getIndices() {
    return indices;
}

int Cube::getNumVertices() {
    return numVertices;
}

int Cube::getNumIndices() {
    return numIndices;
}
