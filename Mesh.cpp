#include "Mesh.h"

void Mesh::resetSurface(Surface *surface) {
    vertices.clear();
    triangles.clear();
    addSurface(surface);
}

void Mesh::addSurface(Surface *surface) {
    int oldVerticesSize = vertices.size();
    int numSurfaceVertices = surface->getNumVertices();
    Vertex* surfaceVertices = surface->getVertices();
    for (int i=0; i<numSurfaceVertices; i++) {
        vertices.push_back(&surfaceVertices[i]);
    }

    int numSurfaceTriangles = surface->getNumTriangles();
    Triangle* surfaceTriangles = surface->getTriangles();
    for (int i=0; i<numSurfaceTriangles; i++) {
        surfaceTriangles[i].vertexIndices[0] += oldVerticesSize;
        surfaceTriangles[i].vertexIndices[1] += oldVerticesSize;
        surfaceTriangles[i].vertexIndices[2] += oldVerticesSize;
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

void Mesh::deleteTriangleReferences(Triangle* triangleToDelete) {
    for (int vertexIndex : triangleToDelete->vertexIndices) {
        Vertex *vertex = vertices[vertexIndex];
        std::set<Triangle *> newTriangles;
        for (Triangle *vertexTriangle: vertex->triangles) {
            if (vertexTriangle != triangleToDelete) {
                newTriangles.insert(vertexTriangle);
            }
        }
        vertex->triangles = newTriangles;
    }
}

void Mesh::deleteVertices(std::unordered_set<Vertex*> verticesToDelete) {
    std::vector<Triangle*> newTriangles;
    for (Triangle* triangle : triangles) {
        Vertex* vertex1 = vertices[triangle->vertexIndices[0]];
        Vertex* vertex2 = vertices[triangle->vertexIndices[1]];
        Vertex* vertex3 = vertices[triangle->vertexIndices[2]];

        if (verticesToDelete.find(vertex1) != verticesToDelete.end() || verticesToDelete.find(vertex2) != verticesToDelete.end() || verticesToDelete.find(vertex3) != verticesToDelete.end()) {
            deleteTriangleReferences(triangle);
        } else {
            newTriangles.push_back(triangle);
        }
    }
    triangles = newTriangles;
}
