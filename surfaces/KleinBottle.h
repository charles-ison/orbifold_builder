#ifndef KLEIN_BOTTLE_H
#define KLEIN_BOTTLE_H

#include "Vertex.h"
#include "Surface.h"

class KleinBottle : public Surface {

public:
    KleinBottle(QVector3D centerPosition, QVector3D scale);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(QVector3D centerPosition, QVector3D scale);
    void initTriangles();
    int floorMod(int a, int n);
    static const int numVertSteps = 180;
    static const int numHorSteps = 180;
    static const int numVertices = 2 * numVertSteps * numHorSteps;
    static const int numTriangles = 2 * numVertices;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
