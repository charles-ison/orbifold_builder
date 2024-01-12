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
    static const int numVertSteps = 50;
    static const int numHorSteps = 50;
    float bigRadius = 1;
    float smallRadius = 0.5;
    static const int numVertices = (numVertSteps+1) * numHorSteps;
    static const int numIndices = 6 * (numVertSteps+1) * numHorSteps;
    VertexData vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
