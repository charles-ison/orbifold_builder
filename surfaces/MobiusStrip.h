#ifndef MOBIUS_STRIP_H
#define MOBIUS_STRIP_H

#include <QOpenGLFunctions>
#include "VertexData.h"
#include "Surface.h"

class MobiusStrip : public Surface {

public:
    MobiusStrip();
    GLushort* getIndices();
    VertexData* getVertices();
    int getNumVertices();
    int getNumIndices();

private:
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float radius = 1.0;
    static const int numVertices = (numVertSteps+1) * (numHorSteps+1);
    static const int numIndices = 6 * (numVertSteps+1) * numHorSteps;
    VertexData vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
