#ifndef SPHERE_H
#define SPHERE_H

#include "Vertex.h"
#include "Surface.h"

class Sphere : public Surface {

public:
    Sphere();
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices();
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float radius = 1.3;
    static const int numVertices = (numVertSteps-1) * numHorSteps + 2;
    static const int numTriangles = 2 * (numVertSteps-1) * numHorSteps;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
