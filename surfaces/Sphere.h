#ifndef SPHERE_H
#define SPHERE_H

#include <QOpenGLFunctions>
#include "VertexData.h"
#include "Surface.h"

class Sphere : public Surface {

public:
    GLushort* getIndices();
    VertexData* getVertices();
    int getNumVertices();
    int getNumIndices();

private:
    int numVertSteps = 50;
    int numHorSteps = 50;
    float radius = 1;
    int numVertices = (numVertSteps+1) * numHorSteps;
    int numIndices = 6 * (numVertSteps+1) * numHorSteps;
    VertexData *vertices;
};

#endif
