#ifndef TORUS_H
#define TORUS_H

#include <QOpenGLFunctions>
#include "VertexData.h"
#include "Surface.h"

class Torus : public Surface {

public:
    Torus();
    GLushort* getIndices();
    VertexData* getVertices();
    int getNumVertices();
    int getNumIndices();

private:
    static const int numVertSteps = 80;
    static const int numHorSteps = 80;
    float bigRadius = 1;
    float smallRadius = 0.5;
    static const int numVertices = numVertSteps * numHorSteps;
    static const int numIndices = 6 * numVertSteps * numHorSteps;
    VertexData vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
