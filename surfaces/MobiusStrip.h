#ifndef MOBIUS_STRIP_H
#define MOBIUS_STRIP_H

#include "Vertex.h"
#include "Surface.h"

class MobiusStrip : public Surface {

public:
    MobiusStrip(float centerX, float centerY, float centerZ);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(float centerX, float centerY, float centerZ);
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float radius = 1.0;
    static const int numVertices = (numVertSteps+1) * (numHorSteps+1);
    static const int numTriangles = 2 * (numVertSteps+1) * numHorSteps;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
