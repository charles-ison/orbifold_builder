#ifndef CROSS_CAP_H
#define CROSS_CAP_H

#include <QOpenGLFunctions>
#include "VertexData.h"
#include "Surface.h"

// TODO: What about the Boy and Roman surfaces as other alternatives for the real projective plane
class CrossCap : public Surface {

public:
    CrossCap();
    GLushort* getIndices();
    VertexData* getVertices();
    int getNumVertices();
    int getNumIndices();

private:
    static const int numVertSteps = 80;
    static const int numHorSteps = 80;
    float radius = 1.3;
    static const int numVertices = (numVertSteps+1) * (numHorSteps+1);
    static const int numIndices = 6 * (numVertSteps+1) * numHorSteps;
    VertexData vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
