#ifndef CUBE_H
#define CUBE_H

#include <QOpenGLFunctions>
#include "Vertex.h"
#include "Surface.h"

class Cube : public Surface {

public:
    Cube();
    GLushort* getIndices();
    Vertex* getVertices();
    std::vector<Triangle*> getTriangles();
    int getNumVertices();
    int getNumIndices();

private:
    void initVertices();
    void initIndices();
    void initTriangles();
    static const int numVertices = 8;
    static const int numIndices = 36;
    Vertex vertices[numVertices];
    GLushort indices[numIndices];
};

#endif
