#ifndef KLEIN_BOTTLE_H
#define KLEIN_BOTTLE_H

#include <QOpenGLFunctions>
#include "VertexData.h"
#include "Surface.h"

class KleinBottle : public Surface {

public:
    KleinBottle();
    GLushort* getIndices();
    VertexData* getVertices();
    int getNumVertices();
    int getNumIndices();

private:
    static const int numVertSteps = 100;
    static const int numHorSteps = 100;
    float a = 0.5;
    float b = 1.0;
    float c = 0.5;
    static const int numVertices = 2 * (numVertSteps+1) * (numHorSteps+1);
    static const int numIndices = 2 * 6 * (numVertSteps+1) * (numHorSteps+1);
    VertexData vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
