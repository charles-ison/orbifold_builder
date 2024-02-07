#ifndef CROSS_CAP_H
#define CROSS_CAP_H

#include "Vertex.h"
#include "Surface.h"

class CrossCap : public Surface {

public:
    CrossCap(float centerX, float centerY, float centerZ);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(float centerX, float centerY, float centerZ);
    void initTriangles();
    static const int numVertSteps = 100;
    static const int numHorSteps = 100;
    float sizeX = 1.5;
    float sizeY = 1.0;
    float sizeZ = 1.0;
    static const int numVertices = (numVertSteps+1) * numHorSteps;
    static const int numTriangles = 2 * numVertices;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
