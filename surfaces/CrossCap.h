#ifndef CROSS_CAP_H
#define CROSS_CAP_H

#include "Vertex.h"
#include "Surface.h"

class CrossCap : public Surface {

public:
    CrossCap();
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices();
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float x_size = 1.0;
    float y_size = 1.5;
    float z_size = 1.0;
    static const int numVertices = (numVertSteps+1) * numHorSteps;
    static const int numTriangles = 2 * numVertices;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
