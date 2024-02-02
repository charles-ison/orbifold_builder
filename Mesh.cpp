#include "Mesh.h"

void Mesh::copySurface(Surface *surface) {
    vertices.clear();
    triangles.clear();

    int numVertices = surface->getNumVertices();
    Vertex* surfaceVertices = surface->getVertices();
    for (int i=0; i<numVertices; i++) {
        vertices.push_back(&surfaceVertices[i]);
    }

    int numTriangles = surface->getNumTriangles();
    Triangle* surfaceTriangles = surface->getTriangles();
    for (int i=0; i<numTriangles; i++) {
        triangles.push_back(&surfaceTriangles[i]);
    }
}

std::vector<Vertex*> Mesh::getVertices() {
    return vertices;
}

std::vector<Triangle*> Mesh::getTriangles() {
    return triangles;
}

void Mesh::addVertex(Vertex *vertexToAdd) {
    vertices.push_back(vertexToAdd);
}

//TODO: clean this up
void Mesh::deleteTriangleReferences(Triangle* triangle) {
    for (int vertexIndex : triangle->vertexIndices) {
        Vertex *vertex = vertices[vertexIndex];
        auto itr = vertex->triangles.begin();
        while (itr != vertex->triangles.end()) {
            if (triangle == *itr) {
                itr = vertex->triangles.erase(itr);
            } else {
                itr++;
            }
        }
    }
}

void Mesh::deleteVertices(std::unordered_set<Vertex*> verticesToDelete) {
    auto itr = triangles.begin();
    while (itr != triangles.end()) {
        Vertex* vertex1 = vertices[(*itr)->vertexIndices[0]];
        Vertex* vertex2 = vertices[(*itr)->vertexIndices[1]];
        Vertex* vertex3 = vertices[(*itr)->vertexIndices[2]];

        if (verticesToDelete.find(vertex1) != verticesToDelete.end() || verticesToDelete.find(vertex2) != verticesToDelete.end() || verticesToDelete.find(vertex3) != verticesToDelete.end()) {
            deleteTriangleReferences(*itr);
            itr = triangles.erase(itr);
        } else {
            itr++;
        }
    }
}
