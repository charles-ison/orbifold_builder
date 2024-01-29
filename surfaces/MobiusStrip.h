#ifndef MOBIUS_STRIP_H
#define MOBIUS_STRIP_H

#include "Vertex.h"
#include "Surface.h"

class MobiusStrip : public Surface {

public:
    MobiusStrip();
    Vertex* getVertices();
    std::vector<Triangle> getTriangles();
    int getNumVertices();

private:
    void initVertices();
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    float radius = 1.0;
    static const int numVertices = (numVertSteps+1) * (numHorSteps+1);
    Vertex vertices[numVertices];
    std::vector<Triangle> triangles;
};

#endif
