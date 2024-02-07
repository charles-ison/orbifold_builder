#ifndef CUBE_H
#define CUBE_H

#include "Vertex.h"
#include "Surface.h"

class Cube : public Surface {

public:
    Cube(QVector3D centerPosition, QVector3D scale);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(QVector3D centerPosition, QVector3D scale);
    void initTriangles();
    static const int numVertices = 8;
    static const int numTriangles = 12;
    Vertex vertices[numVertices];
    Triangle triangles[numTriangles];
};

#endif
