#ifndef TORUS_H
#define TORUS_H

#include "Vertex.h"
#include "Surface.h"

class Torus : public Surface {

public:
    Torus(QVector3D centerPosition, QVector3D scale);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(QVector3D centerPosition, QVector3D scale);
    void initTriangles();
    static const int numVertSteps = 200;
    static const int numHorSteps = 200;
    static const int numVertices = numVertSteps * numHorSteps;
    static const int numTriangles= 2 * numVertices;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
