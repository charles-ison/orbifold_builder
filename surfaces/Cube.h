#ifndef CUBE_H
#define CUBE_H

#include "Vertex.h"
#include "Surface.h"

class Cube : public Surface {

public:
    Cube();
    Vertex* getVertices();
    std::vector<Triangle> getTriangles();
    int getNumVertices();

private:
    void initVertices();
    void initTriangles();
    static const int numVertices = 8;
    Vertex vertices[numVertices];
    std::vector<Triangle> triangles;
};

#endif
