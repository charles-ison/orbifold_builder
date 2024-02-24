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
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};

#endif
