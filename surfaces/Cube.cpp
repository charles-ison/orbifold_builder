#include "Cube.h"

VertexData* Cube::getVertices() {
    VertexData vertices[] = {
            {QVector3D(1.0f,  -1.0f,  1.0f)}, //v0
            {QVector3D( 1.0f,  1.0f,  1.0f)}, //v1
            {QVector3D(-1.0f,  1.0f, 1.0f)}, //v2
            {QVector3D( -1.0f,  -1.0f, 1.0f)}, //v3
            {QVector3D(1.0f,  -1.0f,  -1.0f)}, //v4
            {QVector3D( 1.0f,  1.0f,  -1.0f)}, //v5
            {QVector3D(-1.0f,  1.0f, -1.0f)}, //v6
            {QVector3D( -1.0f,  -1.0f, -1.0f)} //v7
    };
    return vertices;
}

GLushort* Cube::getIndices() {
    GLushort indices[] = {
            0, 1, 2, 2, 3, 0, // F0
            3, 2, 6, 6, 7, 3, // F1
            3, 7, 4, 4, 0, 3, // F2
            4, 7, 6, 6, 5, 4, // F3
            0, 4, 5, 5, 1, 0, // F4
            1, 5, 6, 6, 2, 1 // F5
    };
    return indices;
}

int Cube::getNumVertices() {
    return numVertices;
}

int Cube::getNumIndices() {
    return numIndices;
}
