#ifndef CROSS_CAP_H
#define CROSS_CAP_H

#include "Vertex.h"
#include "Surface.h"

class CrossCap : public Surface {

public:
    CrossCap(QVector3D centerPosition, QVector3D scale);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(QVector3D centerPosition, QVector3D scale);
    void initTriangles();
    static const int numVertSteps = 100;
    static const int numHorSteps = 100;
    static const int numVertices = (numVertSteps+1) * numHorSteps;
    static const int numTriangles = 2 * numVertices;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
