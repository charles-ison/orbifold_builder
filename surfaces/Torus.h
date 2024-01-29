#ifndef TORUS_H
#define TORUS_H

#include "Vertex.h"
#include "Surface.h"

class Torus : public Surface {

public:
    Torus();
    Vertex* getVertices();
    std::vector<Triangle> getTriangles();
    int getNumVertices();

private:
    void initVertices();
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float bigRadius = 1;
    float smallRadius = 0.5;
    static const int numVertices = numVertSteps * numHorSteps;
    Vertex vertices[numVertices];
    std::vector<Triangle> triangles;
};

#endif
