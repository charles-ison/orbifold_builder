#ifndef SPHERE_H
#define SPHERE_H

#include <QOpenGLFunctions>
#include "VertexData.h"
#include "Surface.h"

class Sphere : public Surface {

public:
    Sphere();
    GLushort* getIndices();
    VertexData* getVertices();
    int getNumVertices();
    int getNumIndices();

private:
    static const int numVertSteps = 50;
    static const int numHorSteps = 50;
    float radius = 1;
    static const int numVertices = (numVertSteps+1) * numHorSteps;
    static const int numIndices = 6 * (numVertSteps+1) * numHorSteps;
    VertexData vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
