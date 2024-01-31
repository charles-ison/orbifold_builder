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

// TODO: Remove if element splitting is not required
void Mesh::addVertex(Vertex *vertexToAdd) {
    vertices.push_back(vertexToAdd);
}

// TODO: Remove if element splitting is not required
void Mesh::updateTriangles(int triangleIndex, int index, int newVertexIndex) {
    //TODO: Need to handle deleting out of date triangles
    int oldVertexIndex = triangles[triangleIndex]->vertexIndices[index];
    int secondVertexIndex = triangles[triangleIndex]->vertexIndices[(index+1)%3];
    int thirdVertexIndex = triangles[triangleIndex]->vertexIndices[(index+2)%3];

    //for (auto neighbor : vertices[oldVertexIndex]->neighbors) {
    //    auto itr = neighbor->neighbors.begin();
    //    while (itr != neighbor->neighbors.end()) {
    //        if(*itr == vertices[oldVertexIndex]) {
    //            itr = neighbor->neighbors.erase(itr);
    //        } else {
    //            itr++;
    //        }
    //    }
    //}

    //triangles[triangleIndex]->vertexIndices[index] = newVertexIndex;

    //vertices[newVertexIndex]->neighbors.insert(vertices[secondVertexIndex]);
    //vertices[newVertexIndex]->neighbors.insert(vertices[thirdVertexIndex]);

    //vertices[secondVertexIndex]->neighbors.insert(vertices[newVertexIndex]);
    //vertices[secondVertexIndex]->neighbors.insert(vertices[thirdVertexIndex]);

    //vertices[thirdVertexIndex]->neighbors.insert(vertices[newVertexIndex]);
    //vertices[thirdVertexIndex]->neighbors.insert(vertices[secondVertexIndex]);
}

int Mesh::getUpdatedIndex(int deletedIndex, int index) {
    if (index > deletedIndex) {
        index -= 1;
    }
    return index;
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
        int index1 = (*itr)->vertexIndices[0];
        int index2 = (*itr)->vertexIndices[1];
        int index3 = (*itr)->vertexIndices[2];

        (*itr)->vertexIndices[0] = getUpdatedIndex(deletedIndex, index1);
        (*itr)->vertexIndices[1] = getUpdatedIndex(deletedIndex, index2);
        (*itr)->vertexIndices[2] = getUpdatedIndex(deletedIndex, index3);

        if (deletedIndex == index1 || deletedIndex == index2 || deletedIndex == index3) {
            deleteTriangleReferences(*itr);
            itr = triangles.erase(itr);
        } else {
            itr++;
        }
    }
}
