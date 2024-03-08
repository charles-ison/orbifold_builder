#include "Mesh.h"

void Mesh::resetSurface(Surface *surface) {
    vertices.clear();
    triangles.clear();
    addSurface(surface);
}

void Mesh::addSurface(Surface *surface) {
    int numSurfaceVertices = surface->getNumVertices();
    Vertex* surfaceVertices = surface->getVertices();
    for (int i=0; i<numSurfaceVertices; i++) {
        vertices.push_back(&surfaceVertices[i]);
    }

    int numSurfaceTriangles = surface->getNumTriangles();
    Triangle* surfaceTriangles = surface->getTriangles();
    for (int i=0; i<numSurfaceTriangles; i++) {
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
    for (Vertex* vertex : triangleToDelete->vertices) {
        std::set<Triangle *> newTriangles;
        for (Triangle *vertexTriangle: vertex->triangles) {
            if (vertexTriangle != triangleToDelete) {
                newTriangles.insert(vertexTriangle);
            }
        }
        vertex->triangles = newTriangles;
    }
}

void Mesh::deleteVerticesReferences(std::unordered_set<Vertex*> verticesToDelete) {
    std::vector<Triangle*> newTriangles;
    for (Triangle* triangle : triangles) {
        Vertex* vertex1 = triangle->vertices[0];
        Vertex* vertex2 = triangle->vertices[1];
        Vertex* vertex3 = triangle->vertices[2];

        if (verticesToDelete.find(vertex1) != verticesToDelete.end() || verticesToDelete.find(vertex2) != verticesToDelete.end() || verticesToDelete.find(vertex3) != verticesToDelete.end()) {
            deleteTriangleReferences(triangle);
        } else {
            newTriangles.push_back(triangle);
        }
    }
    triangles = newTriangles;
}

void Mesh::deleteVertices(std::unordered_set<Vertex*> verticesToDelete) {
    int delta = 0;
    auto itr = vertices.begin();
    while(itr != vertices.end()) {
        Vertex* vertex = *itr;
        if(verticesToDelete.find(vertex) != verticesToDelete.end()) {
            itr = vertices.erase(itr);
            delta += 1;
        } else {
            vertex->index -= delta;
            itr ++;
        }
    }
}
