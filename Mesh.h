#ifndef MESH_H
#define MESH_H

#include "surfaces/Vertex.h"
#include "surfaces/Surface.h"

class Mesh {

public:
    void copySurface(Surface *surface);
    std::vector<Vertex*> getVertices();
    std::vector<Triangle*> getTriangles();
    void addVertex(Vertex *vertexToAdd);
    void deleteVertex(Vertex *vertexToDelete);
    void updateTriangles(int triangleIndex, int index, int newVertexIndex);

private:
    void deleteTriangleReferences(Triangle* triangle);
    std::vector<Vertex*> vertices;
    std::vector<Triangle*> triangles;
};

#endif
