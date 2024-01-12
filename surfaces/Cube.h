#ifndef CUBE_H
#define CUBE_H

#include <QOpenGLFunctions>
#include "VertexData.h"

class Cube {

public:
    GLushort* getIndices();
    VertexData* getVertices();
    int getNumVertices();
    int getNumIndices();

private:
    int numVertices = 8;
    int numIndices = 36;
};

#endif
