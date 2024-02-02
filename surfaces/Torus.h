#ifndef TORUS_H
#define TORUS_H

#include "Vertex.h"
#include "Surface.h"

class Torus : public Surface {

public:
    Torus();
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices();
    void initTriangles();
    static const int numVertSteps = 6;
    static const int numHorSteps = 6;
    float bigRadius = 1;
    float smallRadius = 0.5;
    static const int numVertices = numVertSteps * numHorSteps;
    static const int numTriangles= 2 * numVertSteps * numHorSteps;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
