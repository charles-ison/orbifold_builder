#ifndef TORUS_H
#define TORUS_H

#include <QOpenGLFunctions>
#include "Vertex.h"
#include "Surface.h"

class Torus : public Surface {

public:
    Torus();
    GLushort* getIndices();
    Vertex* getVertices();
    std::vector<Triangle> getTriangles();
    int getNumVertices();
    int getNumIndices();

private:
    void initVertices();
    void initIndices();
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float bigRadius = 1;
    float smallRadius = 0.5;
    static const int numVertices = numVertSteps * numHorSteps;
    static const int numIndices = 6 * numVertSteps * numHorSteps;
    Vertex vertices[numVertices];
    GLushort indices[numIndices];
    std::vector<Triangle> triangles;
};

#endif
