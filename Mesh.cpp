#include "Mesh.h"
#include <iostream>

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

void Mesh::deleteVertex(Vertex *vertexToDelete) {
    auto testIndexItr = std::find(vertices.begin(), vertices.end(), vertexToDelete);
    int deletedIndex = testIndexItr - vertices.begin();

    auto itr = triangles.begin();
    while (itr != triangles.end()) {
        int index1 = (*itr)->vertexIndices[0];
        int index2 = (*itr)->vertexIndices[1];
        int index3 = (*itr)->vertexIndices[2];

        if (deletedIndex == index1 || deletedIndex == index2 || deletedIndex == index3) {
            deleteTriangleReferences(*itr);
            itr = triangles.erase(itr);
        } else {
            itr++;
        }
    }
}
