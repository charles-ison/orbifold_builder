#ifndef MESH_H
#define MESH_H

#include "surfaces/Vertex.h"
#include "surfaces/Surface.h"

class Mesh {

public:
    void copySurface(Surface *surface);
    std::vector<Vertex*> getVertices();
    std::vector<Triangle> getTriangles();
    void cutVertex(Vertex *vertexToCut);

private:
    int getUpdatedIndex(int deletedIndex, int index);
    std::vector<Vertex*> vertices;
    std::vector<Triangle> triangles;
};

#endif
