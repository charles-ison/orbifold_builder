#ifndef SPHERE_H
#define SPHERE_H

#include <QOpenGLFunctions>
#include "Vertex.h"
#include "Surface.h"

class Sphere : public Surface {

public:
    Sphere();
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
    static const int numVertices = (numVertSteps-1) * numHorSteps + 2;
    static const int numIndices = 6 * (numVertSteps-1) * numHorSteps;
    Vertex vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
