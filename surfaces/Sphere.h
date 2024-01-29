#ifndef SPHERE_H
#define SPHERE_H

#include "Vertex.h"
#include "Surface.h"

class Sphere : public Surface {

public:
    Sphere();
    Vertex* getVertices();
    std::vector<Triangle> getTriangles();
    int getNumVertices();

private:
    void initVertices();
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float radius = 1.3;
    static const int numVertices = (numVertSteps-1) * numHorSteps + 2;
    Vertex vertices[numVertices];
    std::vector<Triangle> triangles;
};

#endif
