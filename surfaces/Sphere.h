#ifndef SPHERE_H
#define SPHERE_H

#include "Vertex.h"
#include "Surface.h"

class Sphere : public Surface {

public:
    Sphere(QVector3D centerPosition, QVector3D scale);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(QVector3D centerPosition, QVector3D scale);
    void initTriangles();
    static const int numVertSteps = 100;
    static const int numHorSteps = 100;
    static const int numVertices = (numVertSteps-1) * numHorSteps + 2;
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};

#endif
