#include "Mesh.h"

void Mesh::copySurface(Surface *surface) {
    vertices.clear();
    indices.clear();

    int numVertices = surface->getNumVertices();
    VertexData* surfaceVertices = surface->getVertices();
    for (int i=0; i<numVertices; i++) {
        vertices.push_back(&surfaceVertices[i]);
    }

    int numIndices = surface->getNumIndices();
    GLushort* surfaceIndices = surface->getIndices();
    for (int i=0; i<numIndices; i++) {
        indices.push_back(surfaceIndices[i]);
    }
}

std::vector<VertexData*> Mesh::getVertices() {
    return vertices;
}

std::vector<GLushort> Mesh::getIndices() {
    return indices;
}

void Mesh::cutVertex(VertexData *vertexToCut) {
    int deletedIndex = 0;
    for (auto itr = vertices.begin(); itr != vertices.end(); ++itr) {
        if (vertexToCut == *itr) {
            vertices.erase(itr);
            break;
        }
        deletedIndex++;
    }

    int i = 0;
    int numIndicesRemoved = 0;
    while ((i+numIndicesRemoved)<indices.size()) {
        while (deletedIndex == indices[i+numIndicesRemoved] || deletedIndex == indices[i+numIndicesRemoved+1] || deletedIndex == indices[i+numIndicesRemoved+2]) {
            numIndicesRemoved += 3;
        }
        if ((i+numIndicesRemoved)>=indices.size()) {
            break;
        }

        int index1 = indices[i+numIndicesRemoved];
        if (index1 > deletedIndex) {
            index1 -= 1;
        }
        indices[i] = index1;

        int index2 = indices[i+numIndicesRemoved+1];
        if (index2 > deletedIndex) {
            index2 -= 1;
        }
        indices[i+1] = index2;

        int index3 = indices[i+numIndicesRemoved+2];
        if (index3 > deletedIndex) {
            index3 -= 1;
        }
        indices[i+2] = index3;

        i+=3;
    }

    for (int i=0; i<numIndicesRemoved; i++) {
        indices.pop_back();
    }
}
