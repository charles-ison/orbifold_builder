#ifndef PLY_FILE_H
#define PLY_FILE_H

#include "Vertex.h"
#include "Surface.h"

class PlyFile : public Surface {

public:
    PlyFile(QString fileName, QVector3D centerPosition, QVector3D scale);
    Vertex* getVertices();
    Triangle* getTriangles();
    int getNumVertices();
    int getNumTriangles();

private:
    void initVertices(QVector3D centerPosition, QVector3D scale);
    void initTriangles();

    int numVertices;
    int numTriangles;
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    //Vertex vertices[numVertices];
    //Triangle triangles[numTriangles];
};

#endif
