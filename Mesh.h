#ifndef MESH_H
#define MESH_H

#include <unordered_set>
#include "surfaces/Vertex.h"
#include "surfaces/Surface.h"

class Mesh {

public:
    void resetSurface(Surface *surface);
    void addSurface(Surface *surface);
    std::vector<Vertex*> getVertices();
    std::vector<Triangle*> getTriangles();
    void addVertex(Vertex *vertexToAdd);
    void deleteVerticesReferences(std::unordered_set<Vertex*> verticesToDelete);
    void deleteVertices(std::unordered_set<Vertex*> verticesToDelete);
    void deleteOutdatedTriangles();

private:
    void deleteTriangleReferences(Triangle* triangle);
    std::vector<Vertex*> vertices;
    std::vector<Triangle*> triangles;
};

#endif
