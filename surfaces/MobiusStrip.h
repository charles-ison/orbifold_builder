#ifndef MOBIUS_STRIP_H
#define MOBIUS_STRIP_H

#include "Vertex.h"
#include "Surface.h"

class MobiusStrip : public Surface {

public:
    MobiusStrip(QVector3D centerPosition, QVector3D scale);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(QVector3D centerPosition, QVector3D scale);
    void initTriangles();
    static const int numVertSteps = 3;
    static const int numHorSteps = 3;
    static const int numVertices = numVertSteps * numHorSteps;
    static const int numTriangles = 2 * (numVertSteps-1) * numHorSteps;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
