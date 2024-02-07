#ifndef SPHERE_H
#define SPHERE_H

#include "Vertex.h"
#include "Surface.h"

class Sphere : public Surface {

public:
    Sphere(float centerX, float centerY, float centerZ);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(float centerX, float centerY, float centerZ);
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float x_size = 1.3;
    float y_size = 1.3;
    float z_size = 1.3;
    static const int numVertices = (numVertSteps-1) * numHorSteps + 2;
    static const int numTriangles = 2 * (numVertSteps-1) * numHorSteps;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
