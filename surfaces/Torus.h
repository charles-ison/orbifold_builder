#ifndef TORUS_H
#define TORUS_H

#include "Vertex.h"
#include "Surface.h"

class Torus : public Surface {

public:
    Torus(float centerX, float centerY, float centerZ);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(float centerX, float centerY, float centerZ);
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float bigRadius = 1;
    float smallRadius = 0.5;
    static const int numVertices = numVertSteps * numHorSteps;
    static const int numTriangles= 2 * numVertices;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
