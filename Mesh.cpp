#include "Mesh.h"

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

void Mesh::addVertex(Vertex *vertexToAdd) {
    vertices.push_back(vertexToAdd);
}

void Mesh::updateTriangle(int triangleIndex, int index, int newVertexIndex) {
    triangles[triangleIndex].vertexIndices[index] = newVertexIndex;
    vertices[newVertexIndex]->neighbors.insert(vertices[triangles[triangleIndex].vertexIndices[(index+1)%3]]);
    vertices[newVertexIndex]->neighbors.insert(vertices[triangles[triangleIndex].vertexIndices[(index+2)%3]]);
}

int Mesh::getUpdatedIndex(int deletedIndex, int index) {
    if (index > deletedIndex) {
        index -= 1;
    }
    return index;
}

void Mesh::deleteVertex(Vertex *vertexToDelete) {
    int deletedIndex = 0;
    for (auto itr = vertices.begin(); itr != vertices.end(); ++itr) {
        if (vertexToDelete == *itr) {
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
