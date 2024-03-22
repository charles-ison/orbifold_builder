#ifndef MESH_H
#define MESH_H

#include <unordered_set>
#include "surfaces/Vertex.h"
#include "surfaces/Surface.h"
#include "Util.h"

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
    void updateTriangles();

private:
    void deleteTriangleReferences(Triangle* triangle);
    std::vector<double> computeAngles(Vertex* vertex0, Vertex* vertex1, Vertex* vertex2);
    void updateVertexNeighbors();
    std::vector<Vertex*> vertices;
    std::vector<Triangle*> triangles;
};

#endif
