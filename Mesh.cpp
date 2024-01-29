#include "Mesh.h"
#include <iostream>

void Mesh::copySurface(Surface *surface) {
    vertices.clear();
    //indices.clear();
    triangles.clear();

    int numVertices = surface->getNumVertices();
    Vertex* surfaceVertices = surface->getVertices();
    for (int i=0; i<numVertices; i++) {
        vertices.push_back(&surfaceVertices[i]);
    }

    //int numIndices = surface->getNumIndices();
    //GLushort* surfaceIndices = surface->getIndices();
    //for (int i=0; i<numIndices; i++) {
    //    indices.push_back(surfaceIndices[i]);
    //}

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

std::vector<GLushort> Mesh::getIndices() {
    return indices;
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

    //int i = 0;
    //int numIndicesRemoved = 0;
    //while ((i+numIndicesRemoved)<indices.size()) {
    //    while (deletedIndex == indices[i+numIndicesRemoved] || deletedIndex == indices[i+numIndicesRemoved+1] || deletedIndex == indices[i+numIndicesRemoved+2]) {
    //        numIndicesRemoved += 3;
    //    }
    //    if ((i+numIndicesRemoved)>=indices.size()) {
    //        break;
    //    }

    //    int index1 = indices[i+numIndicesRemoved];
    //    int updatedIndex1 = getUpdatedIndex(deletedIndex, index1);
    //    indices[i] = updatedIndex1;

    //    int index2 = indices[i+numIndicesRemoved+1];
    //    int updatedIndex2 = getUpdatedIndex(deletedIndex, index2);
     //   indices[i+1] = updatedIndex2;

    //    int index3 = indices[i+numIndicesRemoved+2];
    //    int updatedIndex3 = getUpdatedIndex(deletedIndex, index3);
     //   indices[i+2] = updatedIndex3;

    //    i+=3;
    //}

    //for (int i=0; i<numIndicesRemoved; i++) {
    //    indices.pop_back();
    //}
}
