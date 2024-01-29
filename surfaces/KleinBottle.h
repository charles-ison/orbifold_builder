#ifndef KLEIN_BOTTLE_H
#define KLEIN_BOTTLE_H

#include <QOpenGLFunctions>
#include "Vertex.h"
#include "Surface.h"

class KleinBottle : public Surface {

public:
    KleinBottle();
    GLushort* getIndices();
    Vertex* getVertices();
    std::vector<Triangle*> getTriangles();
    int getNumVertices();
    int getNumIndices();

private:
    void initVertices();
    void initIndices();
    void initTriangles();
    int floorMod(int a, int n);
    static const int numVertSteps = 180;
    static const int numHorSteps = 180;
    float a = 0.5;
    float b = 1.0;
    float c = 0.5;
    static const int numVertices = 2 * numVertSteps * numHorSteps;
    static const int numIndices = 2 * 6 * numVertSteps * numHorSteps;
    Vertex vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
