#ifndef CROSS_CAP_H
#define CROSS_CAP_H

#include <QOpenGLFunctions>
#include "Vertex.h"
#include "Surface.h"

class CrossCap : public Surface {

public:
    CrossCap();
    GLushort* getIndices();
    Vertex* getVertices();
    std::vector<Triangle*> getTriangles();
    int getNumVertices();
    int getNumIndices();

private:
    void initVertices();
    void initIndices();
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float radius = 1.3;
    static const int numVertices = (numVertSteps+1) * numHorSteps;
    static const int numIndices = 6 * (numVertSteps+1) * numHorSteps;
    Vertex vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
