#ifndef MESH_H
#define MESH_H

#include <unordered_set>
#include "surfaces/Vertex.h"
#include "surfaces/Surface.h"

class Mesh {

public:
    void copySurface(Surface *surface);
    std::vector<Vertex*> getVertices();
    std::vector<Triangle*> getTriangles();
    void addVertex(Vertex *vertexToAdd);
    void deleteVertices(std::unordered_set<Vertex*> verticesToDelete);

private:
    void deleteTriangleReferences(Triangle* triangle);
    std::vector<Vertex*> vertices;
    std::vector<Triangle*> triangles;
};

#endif
