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

    std::vector<Triangle> surfaceTriangles = surface->getTriangles();
    for (int i=0; i<surfaceTriangles.size(); i++) {
        triangles.push_back(surfaceTriangles[i]);
    }
}

std::vector<Vertex*> Mesh::getVertices() {
    return vertices;
}

std::vector<Triangle> Mesh::getTriangles() {
    return triangles;
}

int Mesh::getUpdatedIndex(int deletedIndex, int index) {
    if (index > deletedIndex) {
        index -= 1;
    }
    return index;
}

void Mesh::cutVertex(Vertex *vertexToCut) {
    int deletedIndex = 0;
    for (auto itr = vertices.begin(); itr != vertices.end(); ++itr) {
        if (vertexToCut == *itr) {
            vertices.erase(itr);
            break;
        }
        deletedIndex++;
    }

    auto itr = triangles.begin();
    while (itr != triangles.end()) {
        int index1 = itr->vertexIndices[0];
        int index2 = itr->vertexIndices[1];
        int index3 = itr->vertexIndices[2];

        if (deletedIndex == index1 || deletedIndex == index2 || deletedIndex == index3) {
            itr = triangles.erase(itr);
        } else {
            itr->vertexIndices[0] = getUpdatedIndex(deletedIndex, index1);
            itr->vertexIndices[1] = getUpdatedIndex(deletedIndex, index2);
            itr->vertexIndices[2] = getUpdatedIndex(deletedIndex, index3);
            itr++;
        }
    }
}
